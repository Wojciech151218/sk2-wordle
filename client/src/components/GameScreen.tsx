/**
 * Game Screen - Active game view with Wordle board, keyboard, and player stats
 */

import React, { useState, useEffect, useCallback } from 'react';
import type { GameState, GuessHistory } from '../types';
import { WordleBoard } from './WordleBoard';
import { GameKeyboard } from './GameKeyboard';
import { PlayerStats } from './PlayerStats';

interface GameScreenProps {
  gameState: GameState | null;
  playerName: string;
  onGuess: (guess: string) => Promise<GuessHistory | null>;
  onLeave: () => Promise<void>;
  loading: boolean;
  error: string | null;
}

export const GameScreen: React.FC<GameScreenProps> = ({
  gameState,
  playerName,
  onGuess,
  onLeave,
  loading,
  error,
}) => {
  const [currentGuess, setCurrentGuess] = useState('');
  const [guessHistory, setGuessHistory] = useState<GuessHistory>([]);
  const [localError, setLocalError] = useState<string | null>(null);
  const [isSubmitting, setIsSubmitting] = useState(false);

  const game = gameState?.game;
  const currentPlayer = game?.players_list.find((p) => p.player_name === playerName);
  const isAlive = currentPlayer?.is_alive ?? false;
  const wordLength = 5; // Standard Wordle word length
  const maxAttempts = 6;

  // Calculate time remaining
  const [timeRemaining, setTimeRemaining] = useState(0);
  useEffect(() => {
    if (!gameState) return;

    const updateTime = () => {
      const remaining = Math.max(0, Math.floor(gameState.round_end_time - Date.now() / 1000));
      setTimeRemaining(remaining);
    };

    updateTime();
    const interval = setInterval(updateTime, 1000);
    return () => clearInterval(interval);
  }, [gameState]);

  // Handle keyboard input
  const handleKeyPress = useCallback(
    (key: string) => {
      if (!isAlive || isSubmitting || guessHistory.length >= maxAttempts) return;

      if (currentGuess.length < wordLength) {
        setCurrentGuess((prev) => prev + key.toLowerCase());
        setLocalError(null);
      }
    },
    [isAlive, isSubmitting, currentGuess.length, wordLength, guessHistory.length]
  );

  const handleBackspace = useCallback(() => {
    if (!isAlive || isSubmitting) return;
    setCurrentGuess((prev) => prev.slice(0, -1));
    setLocalError(null);
  }, [isAlive, isSubmitting]);

  const handleEnter = useCallback(async () => {
    if (!isAlive || isSubmitting || guessHistory.length >= maxAttempts) return;

    if (currentGuess.length !== wordLength) {
      setLocalError(`Word must be ${wordLength} letters`);
      return;
    }

    setIsSubmitting(true);
    setLocalError(null);

    try {
      const result = await onGuess(currentGuess);
      if (result) {
        setGuessHistory(result);
        setCurrentGuess('');
      }
    } catch (err) {
      setLocalError(err instanceof Error ? err.message : 'Failed to submit guess');
    } finally {
      setIsSubmitting(false);
    }
  }, [isAlive, isSubmitting, currentGuess, wordLength, onGuess, guessHistory.length]);

  // Listen to physical keyboard
  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      if (!isAlive || isSubmitting) return;

      if (e.key === 'Enter') {
        handleEnter();
      } else if (e.key === 'Backspace') {
        handleBackspace();
      } else if (/^[a-zA-Z]$/.test(e.key)) {
        handleKeyPress(e.key.toUpperCase());
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [isAlive, isSubmitting, handleKeyPress, handleBackspace, handleEnter]);

  if (!game) {
    return (
      <div className="screen game-screen">
        <div className="screen-content">
          <p>Loading game...</p>
        </div>
      </div>
    );
  }

  const currentRound = game.rounds.length;
  const isGameOver = !game.players_list.some((p) => p.is_alive);

  return (
    <div className="screen game-screen">
      <div className="game-layout">
        {/* Left Sidebar - Player Stats */}
        <aside className="game-sidebar">
          <PlayerStats
            players={game.players_list}
            currentPlayerName={playerName}
            roundNumber={currentRound}
          />
        </aside>

        {/* Main Game Area */}
        <main className="game-main">
          <header className="game-header">
            <h1>⚔️ Battle Royale</h1>
            <div className="game-info">
              <div className="info-badge">
                Round {currentRound}
              </div>
              <div className="info-badge">
                ⏱️ {timeRemaining}s
              </div>
            </div>
          </header>

          {isGameOver && (
            <div className="game-over-banner">
              <h2>🎮 Game Over!</h2>
              <p>
                {game.players_list.find((p) => p.is_alive)
                  ? `Winner: ${game.players_list.find((p) => p.is_alive)?.player_name}`
                  : 'No survivors!'}
              </p>
            </div>
          )}

          {!isAlive && !isGameOver && (
            <div className="eliminated-banner">
              <h2>💀 You were eliminated!</h2>
              <p>Watch the remaining players battle it out</p>
            </div>
          )}

          <div className="wordle-container">
            <WordleBoard
              guessHistory={guessHistory}
              currentGuess={currentGuess}
              maxAttempts={maxAttempts}
              wordLength={wordLength}
            />

            {(error || localError) && (
              <div className="error-message">{error || localError}</div>
            )}

            <GameKeyboard
              onKeyPress={handleKeyPress}
              onEnter={handleEnter}
              onBackspace={handleBackspace}
              guessHistory={guessHistory}
              disabled={!isAlive || isSubmitting || isGameOver || guessHistory.length >= maxAttempts}
            />
          </div>

          <div className="game-actions">
            <button onClick={onLeave} className="btn btn-secondary" disabled={loading}>
              Leave Game
            </button>
          </div>
        </main>
      </div>
    </div>
  );
};

