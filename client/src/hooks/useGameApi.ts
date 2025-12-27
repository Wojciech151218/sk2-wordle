/**
 * React hook for Wordle Game API
 * Provides abstracted methods for all API endpoints
 */

import { useState, useCallback, useMemo } from 'react';
import axios, { AxiosError, type AxiosInstance } from 'axios';
import type {
  JoinRequest,
  JoinResponse,
  LeaveRequest,
  LeaveResponse,
  ReadyRequest,
  ReadyResponse,
  StateRequest,
  StateResponse,
  GuessRequest,
  GuessResponse,
  ApiError,
} from '../types';

// ============================================================================
// Configuration
// ============================================================================

const DEFAULT_BASE_URL = import.meta.env.VITE_API_URL || 'http://localhost:8080';

interface UseGameApiConfig {
  baseUrl?: string;
  timeout?: number;
}

// ============================================================================
// Hook Return Type
// ============================================================================

interface UseGameApiReturn {
  // API Methods
  join: (playerName: string) => Promise<JoinResponse>;
  leave: (playerName: string) => Promise<LeaveResponse>;
  ready: (playerName: string) => Promise<ReadyResponse>;
  getState: (playerName: string) => Promise<StateResponse>;
  guess: (playerName: string, guessWord: string) => Promise<GuessResponse>;
  
  // State
  loading: boolean;
  error: ApiError | null;
  
  // Utilities
  clearError: () => void;
  setBaseUrl: (url: string) => void;
}

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * Get current timestamp in ISO format
 */
const getCurrentTimestamp = (): string => {
  return new Date().toISOString().replace(/\.\d{3}Z$/, "Z");
};

/**
 * Handle axios errors and convert to ApiError
 */
const handleApiError = (error: unknown): ApiError => {
  if (axios.isAxiosError(error)) {
    const axiosError = error as AxiosError<{ message?: string }>;
    return {
      message: axiosError.response?.data?.message || axiosError.message || 'An error occurred',
      status: axiosError.response?.status,
    };
  }
  
  if (error instanceof Error) {
    return {
      message: error.message,
    };
  }
  
  return {
    message: 'An unknown error occurred',
  };
};

// ============================================================================
// Main Hook
// ============================================================================

/**
 * Hook for interacting with the Wordle Game API
 * 
 * @param config - Optional configuration (baseUrl, timeout)
 * @returns Object with API methods, loading state, and error handling
 * 
 * @example
 * const { join, getState, loading, error } = useGameApi();
 * 
 * // Join the game
 * const state = await join('PlayerName');
 * 
 * // Get current state
 * const currentState = await getState('PlayerName');
 */
export const useGameApi = (config: UseGameApiConfig = {}): UseGameApiReturn => {
  const [baseUrl, setBaseUrl] = useState(config.baseUrl || DEFAULT_BASE_URL);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<ApiError | null>(null);

  // Create axios instance with config
  const api: AxiosInstance = useMemo(() => {
    return axios.create({
      baseURL: baseUrl,
      timeout: config.timeout || 10000,
      headers: {
        'Content-Type': 'application/json',
      },
    });
  }, [baseUrl, config.timeout]);

  // Clear error state
  const clearError = useCallback(() => {
    setError(null);
  }, []);

  // ============================================================================
  // API Method: JOIN
  // ============================================================================
  
  const join = useCallback(async (playerName: string): Promise<JoinResponse> => {
    setLoading(true);
    setError(null);
    
    try {
      const request: JoinRequest = {
        player_name: playerName,
      };
      
      const response = await api.post<JoinResponse>('/join', request);
      return response.data;
    } catch (err) {
      const apiError = handleApiError(err);
      setError(apiError);
      throw apiError;
    } finally {
      setLoading(false);
    }
  }, [api]);

  // ============================================================================
  // API Method: LEAVE
  // ============================================================================
  
  const leave = useCallback(async (playerName: string): Promise<LeaveResponse> => {
    setLoading(true);
    setError(null);
    
    try {
      const request: LeaveRequest = {
        player_name: playerName,
      };
      
      const response = await api.delete<LeaveResponse>('/leave', {
        data: request,
      });
      return response.data;
    } catch (err) {
      const apiError = handleApiError(err);
      setError(apiError);
      throw apiError;
    } finally {
      setLoading(false);
    }
  }, [api]);

  // ============================================================================
  // API Method: READY
  // ============================================================================
  
  const ready = useCallback(async (playerName: string): Promise<ReadyResponse> => {
    setLoading(true);
    setError(null);
    
    try {
      const request: ReadyRequest = {
        player_name: playerName,
        timestamp: getCurrentTimestamp(),
      };
      
      const response = await api.post<ReadyResponse>('/ready', request);
      return response.data;
    } catch (err) {
      const apiError = handleApiError(err);
      setError(apiError);
      throw apiError;
    } finally {
      setLoading(false);
    }
  }, [api]);

  // ============================================================================
  // API Method: GET STATE
  // ============================================================================
  
  const getState = useCallback(async (playerName: string): Promise<StateResponse> => {
    setLoading(true);
    setError(null);
    
    try {
      const request: StateRequest = {
        player_name: playerName,
        timestamp: getCurrentTimestamp(),
      };
      
      // Note: GET with body is unusual but matches the API spec
      const response = await api.get<StateResponse>('/', {
        data: request,
      });
      return response.data;
    } catch (err) {
      const apiError = handleApiError(err);
      setError(apiError);
      throw apiError;
    } finally {
      setLoading(false);
    }
  }, [api]);

  // ============================================================================
  // API Method: GUESS
  // ============================================================================
  
  const guess = useCallback(async (
    playerName: string,
    guessWord: string
  ): Promise<GuessResponse> => {
    setLoading(true);
    setError(null);
    
    try {
      const request: GuessRequest = {
        player_name: playerName,
        timestamp: getCurrentTimestamp(),
        guess: guessWord,
      };
      
      const response = await api.post<GuessResponse>('/guess', request);
      return response.data;
    } catch (err) {
      const apiError = handleApiError(err);
      setError(apiError);
      throw apiError;
    } finally {
      setLoading(false);
    }
  }, [api]);

  // ============================================================================
  // Return Hook Interface
  // ============================================================================

  return {
    // API Methods
    join,
    leave,
    ready,
    getState,
    guess,
    
    // State
    loading,
    error,
    
    // Utilities
    clearError,
    setBaseUrl,
  };
};

