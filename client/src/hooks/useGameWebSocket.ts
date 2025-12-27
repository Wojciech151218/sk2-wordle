/**
 * React hook for WebSocket connection to receive real-time game state updates
 * Connects to the backend WebSocket server and receives GameState broadcasts
 */

import { useState, useEffect, useCallback, useRef } from 'react';
import type { GameState } from '../types';
// ============================================================================
// Configuration
// ============================================================================

const DEFAULT_WS_URL = import.meta.env.VITE_WS_URL || 'ws://0.0.0.0:4040';
const RECONNECT_DELAY = 3000; // 3 seconds
const MAX_RECONNECT_ATTEMPTS = 10;

interface UseGameWebSocketConfig {
  wsUrl?: string;
  autoConnect?: boolean;
  reconnect?: boolean;
  maxReconnectAttempts?: number;
  reconnectDelay?: number;
  onError?: (error: Event) => void;
  onConnect?: () => void;
  onDisconnect?: () => void;
}

// ============================================================================
// Connection Status Type
// ============================================================================

export type ConnectionStatus = 
  | 'disconnected' 
  | 'connecting' 
  | 'connected' 
  | 'reconnecting' 
  | 'error';

// ============================================================================
// Hook Return Type
// ============================================================================

interface UseGameWebSocketReturn {
  // Current game state from WebSocket
  
  // Connection management
  connectionStatus: ConnectionStatus;
  isConnected: boolean;
  
  // Manual control
  connect: () => void;
  disconnect: () => void;
  reconnect: () => void;
  
  // Stats
  lastMessageTime: Date | null;
  messageCount: number;
  reconnectAttempts: number;
}

// ============================================================================
// Main Hook
// ============================================================================

/**
 * Hook for WebSocket connection to receive real-time game state updates
 * 
 * The backend broadcasts GameState every 60 seconds and on state changes.
 * This hook automatically parses and updates the state.
 * 
 * @param config - Optional configuration for WebSocket connection
 * @returns Object with current game state, connection status, and control methods
 * 
 * @example
 * const { gameState, isConnected, connect, disconnect } = useGameWebSocket();
 * 
 * useEffect(() => {
 *   if (gameState) {
 *     console.log('Game state updated:', gameState);
 *   }
 * }, [gameState]);
 */
export const useGameWebSocket = (
  config: UseGameWebSocketConfig = {},
  setGameState: (gameState: GameState) => void
): UseGameWebSocketReturn => {
  const {
    wsUrl = `${DEFAULT_WS_URL}/ws`,
    autoConnect = true,
    reconnect = true,
    maxReconnectAttempts = MAX_RECONNECT_ATTEMPTS,
    reconnectDelay = RECONNECT_DELAY,
    onError,
    onConnect,
    onDisconnect,
  } = config;

  // ============================================================================
  // State
  // ============================================================================
  
  const [connectionStatus, setConnectionStatus] = useState<ConnectionStatus>('disconnected');
  const [lastMessageTime, setLastMessageTime] = useState<Date | null>(null);
  const [messageCount, setMessageCount] = useState(0);
  const [reconnectAttempts, setReconnectAttempts] = useState(0);

  // ============================================================================
  // Refs (to avoid stale closures)
  // ============================================================================
  
  const wsRef = useRef<WebSocket | null>(null);
  const reconnectTimeoutRef = useRef<ReturnType<typeof setTimeout> | null>(null);
  const reconnectAttemptsRef = useRef(0);
  const intentionalDisconnectRef = useRef(false);

  // ============================================================================
  // Computed Values
  // ============================================================================
  
  const isConnected = connectionStatus === 'connected';

  // ============================================================================
  // WebSocket Event Handlers
  // ============================================================================
  
  const handleOpen = useCallback(() => {
    console.log('[WebSocket] Connected to game server');
    setConnectionStatus('connected');
    setReconnectAttempts(0);
    reconnectAttemptsRef.current = 0;
    
    if (onConnect) {
      onConnect();
    }
  }, [onConnect]);

  const handleMessage = useCallback((event: MessageEvent) => {
    try {
      const data = JSON.parse(event.data);
      
      // Validate that it's a GameState object
      if (data && typeof data === 'object') {
        setGameState(data as GameState);
        setLastMessageTime(new Date());
        setMessageCount(prev => prev + 1);
        
        console.log('[WebSocket] Game state updated:', data);
      } else {
        console.warn('[WebSocket] Received invalid message format:', data);
      }
    } catch (error) {
      console.error('[WebSocket] Failed to parse message:', error);
    }
  }, []);

  const handleError = useCallback((event: Event) => {
    console.error('[WebSocket] Connection error:', event);
    setConnectionStatus('error');
    
    if (onError) {
      onError(event);
    }
  }, [onError]);

  const handleClose = useCallback((event: CloseEvent) => {
    console.log('[WebSocket] Connection closed:', event.code, event.reason);
    setConnectionStatus('disconnected');
    wsRef.current = null;
    
    if (onDisconnect) {
      onDisconnect();
    }

    // Attempt reconnection if enabled and not intentional disconnect
    if (
      reconnect &&
      !intentionalDisconnectRef.current &&
      reconnectAttemptsRef.current < maxReconnectAttempts
    ) {
      reconnectAttemptsRef.current += 1;
      setReconnectAttempts(reconnectAttemptsRef.current);
      setConnectionStatus('reconnecting');
      
      console.log(
        `[WebSocket] Reconnecting in ${reconnectDelay}ms... (Attempt ${reconnectAttemptsRef.current}/${maxReconnectAttempts})`
      );
      
      reconnectTimeoutRef.current = setTimeout(() => {
        connectWebSocket();
      }, reconnectDelay);
    } else if (reconnectAttemptsRef.current >= maxReconnectAttempts) {
      console.error('[WebSocket] Max reconnection attempts reached');
      setConnectionStatus('error');
    }
  }, [reconnect, maxReconnectAttempts, reconnectDelay, onDisconnect]);

  // ============================================================================
  // Connection Management
  // ============================================================================
  
  const connectWebSocket = useCallback(() => {
    // Clean up existing connection
    if (wsRef.current) {
      if (wsRef.current.readyState === WebSocket.OPEN) {
        console.log('[WebSocket] Already connected');
        return;
      }
      wsRef.current.close();
    }

    // Clear any pending reconnect
    if (reconnectTimeoutRef.current) {
      clearTimeout(reconnectTimeoutRef.current);
      reconnectTimeoutRef.current = null;
    }

    try {
      console.log('[WebSocket] Connecting to:', wsUrl);
      setConnectionStatus('connecting');
      intentionalDisconnectRef.current = false;

      const ws = new WebSocket(wsUrl);
      
      ws.onopen = handleOpen;
      ws.onmessage = handleMessage;
      ws.onerror = handleError;
      ws.onclose = handleClose;
      
      wsRef.current = ws;
    } catch (error) {
      console.error('[WebSocket] Failed to create connection:', error);
      setConnectionStatus('error');
    }
  }, [wsUrl, handleOpen, handleMessage, handleError, handleClose]);

  const disconnectWebSocket = useCallback(() => {
    console.log('[WebSocket] Disconnecting...');
    intentionalDisconnectRef.current = true;
    
    // Clear reconnect timeout
    if (reconnectTimeoutRef.current) {
      clearTimeout(reconnectTimeoutRef.current);
      reconnectTimeoutRef.current = null;
    }

    // Close WebSocket connection
    if (wsRef.current) {
      wsRef.current.close();
      wsRef.current = null;
    }

    setConnectionStatus('disconnected');
    setReconnectAttempts(0);
    reconnectAttemptsRef.current = 0;
  }, []);

  const reconnectWebSocket = useCallback(() => {
    console.log('[WebSocket] Manual reconnect requested');
    setReconnectAttempts(0);
    reconnectAttemptsRef.current = 0;
    intentionalDisconnectRef.current = false;
    
    disconnectWebSocket();
    
    // Small delay before reconnecting
    setTimeout(() => {
      connectWebSocket();
    }, 100);
  }, [connectWebSocket, disconnectWebSocket]);

  // ============================================================================
  // Auto-connect on mount
  // ============================================================================
  
  useEffect(() => {
    if (autoConnect) {
      connectWebSocket();
    }

    // Cleanup on unmount
    return () => {
      intentionalDisconnectRef.current = true;
      
      if (reconnectTimeoutRef.current) {
        clearTimeout(reconnectTimeoutRef.current);
      }
      
      if (wsRef.current) {
        wsRef.current.close();
      }
    };
  }, [autoConnect]); // Only run on mount/unmount

  // ============================================================================
  // Return Hook Interface
  // ============================================================================

  return {
    
    // Connection status
    connectionStatus,
    isConnected,
    
    // Control methods
    connect: connectWebSocket,
    disconnect: disconnectWebSocket,
    reconnect: reconnectWebSocket,
    
    // Stats
    lastMessageTime,
    messageCount,
    reconnectAttempts,
  };
};

