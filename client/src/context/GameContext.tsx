import React, { createContext, useCallback, useContext, useEffect, useMemo, useState } from 'react';
import { useGameWebSocket, type ConnectionStatus } from '../hooks/useGameWebSocket';
import { useGameApi } from '../hooks/useGameApi';
import type { GameState, GuessHistory } from '../types';

type AppScreen = 'join' | 'lobby' | 'game';

interface GameContextValue {
  gameState: GameState | null;
  connectionStatus: ConnectionStatus;
  playerName: string | null;
  currentScreen: AppScreen;
  errorMessage: string | null;
  apiLoading: boolean;
  joinGame: (playerName: string) => Promise<void>;
  leaveGame: () => Promise<void>;
  readyUp: () => Promise<void>;
  submitGuess: (guessWord: string) => Promise<GuessHistory | null>;
  clearError: () => void;
}

const GameContext = createContext<GameContextValue | undefined>(undefined);

const PLAYER_NAME_KEY = 'wordle-battle-player-name';

export const GameProvider: React.FC<React.PropsWithChildren> = ({ children }) => {
  const [gameState, setGameState] = useState<GameState | null>(null);
  const [playerName, setPlayerName] = useState<string | null>(() => {
    return localStorage.getItem(PLAYER_NAME_KEY);
  });
  const [currentScreen, setCurrentScreen] = useState<AppScreen>('join');
  const [errorMessage, setErrorMessage] = useState<string | null>(null);

  const {
    connectionStatus,
  } = useGameWebSocket(
    {
      autoConnect: true,
      reconnect: true,
    },
    setGameState
  );

  const {
    join,
    leave,
    ready,
    guess,
    loading: apiLoading,
    error: apiError,
    clearError: clearApiError,
  } = useGameApi();

  const handleScreenChange = useCallback(
    (state: GameState) => {
      // Defensive checks: ensure players_list exists and is an array
      const isPlayerInLobby = state.players_list && Array.isArray(state.players_list)
        ? state.players_list.some((p) => p.player_name === playerName)
        : false;
      const isPlayerInGame = state.game && state.game.players_list && Array.isArray(state.game.players_list)
        ? state.game.players_list.some((p) => p.player_name === playerName)
        : false;

      if (!isPlayerInGame && !isPlayerInLobby) {
        setCurrentScreen('join');
      } else if (isPlayerInLobby) {
        setCurrentScreen('lobby');
      } else {
        setCurrentScreen('game');
      }
    },
    [playerName]
  );

  useEffect(() => {
    if (!playerName) {
      setCurrentScreen('join');
      return;
    }

    if (!gameState) {
      return;
    }

    handleScreenChange(gameState);
  }, [gameState, playerName, handleScreenChange]);

  useEffect(() => {
    if (errorMessage || apiError) {
      const timer = setTimeout(() => {
        setErrorMessage(null);
        clearApiError();
      }, 5000);
      return () => clearTimeout(timer);
    }
  }, [errorMessage, apiError, clearApiError]);

  const joinGame = useCallback(
    async (name: string) => {
      try {
        setErrorMessage(null);
        const response = await join(name);
        setGameState(response);
        handleScreenChange(response);
        setPlayerName(name);
        localStorage.setItem(PLAYER_NAME_KEY, name);
      } catch (err) {
        setErrorMessage(err instanceof Error ? err.message : 'Failed to join game');
        throw err;
      }
    },
    [join, handleScreenChange]
  );

  const leaveGame = useCallback(async () => {
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
  }, [playerName, leave, handleScreenChange]);

  const readyUp = useCallback(async () => {
    if (!playerName) return;

    try {
      setErrorMessage(null);
      const response = await ready(playerName);
      setGameState(response);
      handleScreenChange(response);
    } catch (err) {
      setErrorMessage(err instanceof Error ? err.message : 'Failed to ready up');
      throw err;
    }
  }, [playerName, ready, handleScreenChange]);

  const submitGuess = useCallback(
    async (guessWord: string): Promise<GuessHistory | null> => {
      if (!playerName) return null;

      try {
        setErrorMessage(null);
        const { guess_result, state } = await guess(playerName, guessWord);
        setGameState(state);
        return guess_result;
      } catch (err) {
        setErrorMessage(err instanceof Error ? err.message : 'Failed to submit guess');
        throw err;
      }
    },
    [playerName, guess]
  );

  const clearError = useCallback(() => {
    setErrorMessage(null);
    clearApiError();
  }, [clearApiError]);

  const contextValue: GameContextValue = useMemo(
    () => ({
      gameState,
      connectionStatus,
      playerName,
      currentScreen,
      errorMessage: errorMessage || apiError?.message || null,
      apiLoading,
      joinGame,
      leaveGame,
      readyUp,
      submitGuess,
      clearError,
    }),
    [
      gameState,
      connectionStatus,
      playerName,
      currentScreen,
      errorMessage,
      apiError?.message,
      apiLoading,
      joinGame,
      leaveGame,
      readyUp,
      submitGuess,
      clearError,
    ]
  );

  return <GameContext.Provider value={contextValue}>{children}</GameContext.Provider>;
};

export const useGameContext = (): GameContextValue => {
  const context = useContext(GameContext);
  if (!context) {
    throw new Error('useGameContext must be used within a GameProvider');
  }
  return context;
};

