/**
 * Wordle Board - Displays the guess grid with colored tiles
 */

import React from 'react';
import type { GuessHistory, LetterType } from '../types';

interface WordleBoardProps {
  guessHistory: GuessHistory;
  currentGuess: string;
  maxAttempts?: number;
  wordLength?: number;
}

export const WordleBoard: React.FC<WordleBoardProps> = ({
  guessHistory,
  currentGuess,
  maxAttempts = 6,
  wordLength = 5,
}) => {
  const rows = [];

  // Add completed guesses
  for (let i = 0; i < guessHistory.length; i++) {
    rows.push(
      <div key={`guess-${i}`} className="wordle-row">
        {guessHistory[i].letters.map((letterData, j) => (
          <div
            key={`${i}-${j}`}
            className={`wordle-tile tile-${letterData.type}`}
          >
            {letterData.letter.toUpperCase()}
          </div>
        ))}
      </div>
    );
  }

  // Add current guess row (if game not over)
  if (guessHistory.length < maxAttempts) {
    const currentLetters = currentGuess.toUpperCase().split('');
    rows.push(
      <div key="current" className="wordle-row">
        {Array.from({ length: wordLength }).map((_, i) => (
          <div
            key={`current-${i}`}
            className={`wordle-tile ${currentLetters[i] ? 'tile-filled' : 'tile-empty'}`}
          >
            {currentLetters[i] || ''}
          </div>
        ))}
      </div>
    );
  }

  // Add empty rows
  const remainingRows = maxAttempts - rows.length;
  for (let i = 0; i < remainingRows; i++) {
    rows.push(
      <div key={`empty-${i}`} className="wordle-row">
        {Array.from({ length: wordLength }).map((_, j) => (
          <div key={`${i}-${j}`} className="wordle-tile tile-empty"></div>
        ))}
      </div>
    );
  }

  return <div className="wordle-board">{rows}</div>;
};

