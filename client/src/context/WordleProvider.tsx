import {
  createContext,
  useCallback,
  useContext,
  useEffect,
  useMemo,
  useState,
  type ReactNode,
} from 'react';
import { useWordleApi } from '../hooks/useWordleApi';
import type {
  ConnectionStatus,
  EvaluatedWord,
  LetterState,
  WordlePayload,
} from '../types/wordle';

const DEFAULT_WORD_LENGTH = 5;
const DEFAULT_MAX_WORDS = 6;

type KeyboardHints = Record<string, LetterState>;

type WordleContextValue = {
  lettersPerWord: number;
  maxWords: number;
  guesses: EvaluatedWord[];
  currentGuess: string;
  keyboardHints: KeyboardHints;
  connectionStatus: ConnectionStatus;
  lastError: string | null;
  isSending: boolean;
  addLetter: (letter: string) => void;
  removeLetter: () => void;
  submitGuess: () => void;
  gameState: WordlePayload | null;
};

const WordleContext = createContext<WordleContextValue | null>(null);

export const WordleProvider = ({ children }: { children: ReactNode }) => {
  const { gameState, connectionStatus, lastError, isSending, sendGuess } =
    useWordleApi();

  const [currentGuess, setCurrentGuess] = useState('');
  const lettersPerWord = gameState?.letters_per_word ?? DEFAULT_WORD_LENGTH;
  const maxWords = gameState?.max_words ?? DEFAULT_MAX_WORDS;

  const guessCount = gameState?.words.length ?? 0;

  useEffect(() => {
    setCurrentGuess('');
  }, [guessCount]);

  const addLetter = useCallback(
    (letter: string) => {
      if (currentGuess.length >= lettersPerWord || isSending) return;
      if (!/^[a-zA-Z]$/.test(letter)) return;
      setCurrentGuess((prev) => prev + letter.toLowerCase());
    },
    [currentGuess.length, isSending, lettersPerWord],
  );

  const removeLetter = useCallback(() => {
    if (isSending) return;
    setCurrentGuess((prev) => prev.slice(0, -1));
  }, [isSending]);

  const submitGuess = useCallback(() => {
    if (currentGuess.length !== lettersPerWord || isSending) return;
    sendGuess(currentGuess);
  }, [currentGuess, isSending, lettersPerWord, sendGuess]);

  const keyboardHints = useMemo(() => {
    const hints: KeyboardHints = {};
    const priority: Record<LetterState, number> = {
      green: 3,
      yellow: 2,
      gray: 1,
      unknown: 0,
    };

    (gameState?.words ?? []).forEach((word) => {
      word.letters.forEach((letter) => {
        const key = letter.letter.toLowerCase();
        const existing = hints[key];
        if (!existing || priority[letter.type] > priority[existing]) {
          hints[key] = letter.type;
        }
      });
    });
    return hints;
  }, [gameState?.words]);

  const value: WordleContextValue = {
    lettersPerWord,
    maxWords,
    guesses: gameState?.words ?? [],
    currentGuess,
    keyboardHints,
    connectionStatus,
    lastError,
    isSending,
    addLetter,
    removeLetter,
    submitGuess,
    gameState,
  };

  return (
    <WordleContext.Provider value={value}>{children}</WordleContext.Provider>
  );
};

export const useWordle = () => {
  const ctx = useContext(WordleContext);
  if (!ctx) {
    throw new Error('useWordle must be used within WordleProvider');
  }
  return ctx;
};

