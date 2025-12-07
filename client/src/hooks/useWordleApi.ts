import { useCallback, useState } from 'react';
import axios from 'axios';
import type {
  ConnectionStatus,
  WordlePayload,
} from '../types/wordle';

const API_URL =
  import.meta.env.VITE_API_URL ?? 'http://localhost:8080';

export type UseWordleApiResult = {
  gameState: WordlePayload | null;
  connectionStatus: ConnectionStatus;
  lastError: string | null;
  isSending: boolean;
  sendGuess: (word: string) => void;
};

export const useWordleApi = (): UseWordleApiResult => {
  const [gameState, setGameState] = useState<WordlePayload | null>(null);
  const [connectionStatus, setConnectionStatus] =
    useState<ConnectionStatus>('idle');
  const [lastError, setLastError] = useState<string | null>(null);
  const [isSending, setIsSending] = useState(false);

  const sendGuess = useCallback(
    async (word: string) => {
      setIsSending(true);
      setLastError(null);
      setConnectionStatus('connecting');

      try {
        const response = await axios.post<WordlePayload>(
          `${API_URL}/word`,
          { word },
          {
            headers: {
              'Content-Type': 'application/json',
            },
          }
        );

        setGameState(response.data);
        setConnectionStatus('open');
        setLastError(null);
      } catch (error) {
        const message =
          axios.isAxiosError(error) && error.response?.data
            ? error.response.data.message || error.response.data.error || error.message
            : error instanceof Error
            ? error.message
            : 'Unknown error';
        setLastError(message);
        setConnectionStatus('error');
      } finally {
        setIsSending(false);
      }
    },
    [],
  );

  return {
    gameState,
    connectionStatus,
    lastError,
    isSending,
    sendGuess,
  };
};

