import { useCallback, useState } from 'react';
import type {
  ConnectionStatus,
  WordlePayload,
} from '../types/wordle';

const API_URL =
  import.meta.env.VITE_API_URL ?? 'http://localhost:5174/api';

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
        const url = new URL(API_URL);
        url.searchParams.set('method', 'word');
        url.searchParams.set('word', word);

        const response = await fetch(url.toString(), {
          method: 'GET',
          headers: {
            'Content-Type': 'application/json',
          },
        });

        if (!response.ok) {
          const errorData = await response.json().catch(() => ({
            error: 'Unknown error',
            message: `HTTP ${response.status}`,
          }));
          throw new Error(errorData.message || errorData.error || 'Request failed');
        }

        const data = (await response.json()) as WordlePayload;
        setGameState(data);
        setConnectionStatus('open');
        setLastError(null);
      } catch (error) {
        const message =
          error instanceof Error ? error.message : 'Unknown error';
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

