/**
 * Game Keyboard - Virtual keyboard with letter highlighting
 */

import React from 'react';
import type { GuessHistory, LetterType } from '../types';

interface GameKeyboardProps {
  onKeyPress: (key: string) => void;
  onEnter: () => void;
  onBackspace: () => void;
  guessHistory: GuessHistory;
  disabled?: boolean;
}

const KEYBOARD_ROWS = [
  ['Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'],
  ['A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L'],
  ['ENTER', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', 'BACK'],
];

export const GameKeyboard: React.FC<GameKeyboardProps> = ({
  onKeyPress,
  onEnter,
  onBackspace,
  guessHistory,
  disabled = false,
}) => {
  // Calculate letter states from guess history
  const letterStates = React.useMemo(() => {
    const states = new Map<string, LetterType>();

    guessHistory.forEach((guess) => {
      guess.letters.forEach((letterData) => {
        const letter = letterData.letter.toUpperCase();
        const currentState = states.get(letter);

        // Priority: green > yellow > gray
        if (letterData.type === 'green') {
          states.set(letter, 'green');
        } else if (letterData.type === 'yellow' && currentState !== 'green') {
          states.set(letter, 'yellow');
        } else if (!currentState) {
          states.set(letter, letterData.type);
        }
      });
    });

    return states;
  }, [guessHistory]);

  const handleClick = (key: string) => {
    if (disabled) return;

    if (key === 'ENTER') {
      onEnter();
    } else if (key === 'BACK') {
      onBackspace();
    } else {
      onKeyPress(key);
    }
  };

  return (
    <div className="game-keyboard">
      {KEYBOARD_ROWS.map((row, rowIndex) => (
        <div key={rowIndex} className="keyboard-row">
          {row.map((key) => {
            const letterState = letterStates.get(key);
            const isSpecial = key === 'ENTER' || key === 'BACK';
            
            return (
              <button
                key={key}
                onClick={() => handleClick(key)}
                disabled={disabled}
                className={`keyboard-key ${isSpecial ? 'key-special' : ''} ${
                  letterState ? `key-${letterState}` : 'key-unused'
                }`}
              >
                {key === 'BACK' ? '⌫' : key}
              </button>
            );
          })}
        </div>
      ))}
    </div>
  );
};

