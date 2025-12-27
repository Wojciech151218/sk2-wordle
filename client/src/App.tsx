/**
 * Main App - Orchestrates the entire game UI
 * Manages player state, WebSocket connection, and screen transitions
 */

import { useState, useEffect, useCallback } from 'react';
import { useGameWebSocket } from './hooks/useGameWebSocket';
import { useGameApi } from './hooks/useGameApi';
import { JoinScreen } from './components/JoinScreen';
import { LobbyScreen } from './components/LobbyScreen';
import { GameScreen } from './components/GameScreen';
import type { GameState, GuessHistory } from './types';
import './index.css';

// LocalStorage key for player name
const PLAYER_NAME_KEY = 'wordle-battle-player-name';

type AppScreen = 'join' | 'lobby' | 'game';

function App() {
  // Initialize hooks

  const [gameState, setGameState] = useState<GameState | null>(null);

  const { isConnected } = useGameWebSocket({
    autoConnect: true,
    reconnect: true,
  }, setGameState);

  const {
    join,
    leave,
    ready,
    guess,
    loading,
    error: apiError,
    clearError,
  } = useGameApi();

  // Local state
  const [playerName, setPlayerName] = useState<string | null>(() => {
    return localStorage.getItem(PLAYER_NAME_KEY);
  });
  const [currentScreen, setCurrentScreen] = useState<AppScreen>('join');
  const [errorMessage, setErrorMessage] = useState<string | null>(null);

  const handleScreenChange = useCallback((gameState: GameState) => {
    console.log(gameState);
    const isPlayerInLobby = gameState.players_list.some((p) => p.player_name === playerName);
    const isPlayerInGame = gameState.game  
      ? gameState.game.players_list.some((p) => p.player_name === playerName)
      : false;

    if (!isPlayerInGame && !isPlayerInLobby) {
      setCurrentScreen('join');
    } else if (isPlayerInLobby) {
      setCurrentScreen('lobby');
    } else {
      setCurrentScreen('game');
    }
  }, [playerName]);

  // Determine which screen to show based on game state
  useEffect(() => {
    if (!playerName) {
      setCurrentScreen('join');
      return;
    }

    if (!gameState) {
      // Still loading
      return;
    }

    handleScreenChange(gameState);
    
  }, [gameState, playerName, handleScreenChange]);

  // Clear error messages after 5 seconds
  useEffect(() => {
    if (errorMessage || apiError) {
      const timer = setTimeout(() => {
        setErrorMessage(null);
        clearError();
      }, 5000);
      return () => clearTimeout(timer);
    }
  }, [errorMessage, apiError, clearError]);

  // ============================================================================
  // Action Handlers
  // ============================================================================

  const handleJoin = useCallback(
    async (name: string) => {
      try {
        setErrorMessage(null);
        const response = await join(name);
        setGameState(response);
        handleScreenChange(response);
        console.log(response);
        setPlayerName(name);
        localStorage.setItem(PLAYER_NAME_KEY, name);
      } catch (err) {
        setErrorMessage(err instanceof Error ? err.message : 'Failed to join game');
        throw err;
      }
    },
    [join]
  );

  const handleLeave = useCallback(async () => {
    if (!playerName) return;

    try {
      setErrorMessage(null);
      const response = await leave(playerName);
      setGameState(response);
      setPlayerName(null);
      localStorage.removeItem(PLAYER_NAME_KEY);
      handleScreenChange(response);
    } catch (err) {
      setErrorMessage(err instanceof Error ? err.message : 'Failed to leave game');
      throw err;
    }
  }, [playerName, leave]);

  const handleReady = useCallback(async () => {
    if (!playerName) return;

    try {
      setErrorMessage(null);
      const response = await ready(playerName);
      setGameState(response);
      console.log(response);
      handleScreenChange(response);
    } catch (err) {
      setErrorMessage(err instanceof Error ? err.message : 'Failed to ready up');
      throw err;
    }
  }, [playerName, ready]);

  const handleGuess = useCallback(
    async (guessWord: string): Promise<GuessHistory | null> => {
      if (!playerName) return null;

      try {
        setErrorMessage(null);
        const {guess_result, state} = await guess(playerName, guessWord);
        setGameState(state);
        return guess_result;
      } catch (err) {
        setErrorMessage(err instanceof Error ? err.message : 'Failed to submit guess');
        throw err;
      }
    },
    [playerName, guess, setGameState]
  );

  // ============================================================================
  // Render Screens
  // ============================================================================

  const displayError = errorMessage || apiError?.message || null;

  if (currentScreen === 'join') {
    return (
      <JoinScreen
        gameState={gameState}
        isConnected={isConnected}
        onJoin={handleJoin}
        loading={loading}
        error={displayError}
      />
    );
  }

  if (currentScreen === 'lobby') {
    return (
      <LobbyScreen
        gameState={gameState}
        isConnected={isConnected}
        playerName={playerName!}
        onReady={handleReady}
        onLeave={handleLeave}
        loading={loading}
        error={displayError}
      />
    );
  }

  if (currentScreen === 'game') {
    return (
      <GameScreen
        gameState={gameState}
        playerName={playerName!}
        onGuess={handleGuess}
        onLeave={handleLeave}
        loading={loading}
        error={displayError}
      />
    );
  }

  return null;
}

export default App;
