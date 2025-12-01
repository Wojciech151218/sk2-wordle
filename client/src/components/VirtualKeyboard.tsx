import { useEffect } from 'react';
import clsx from 'clsx';
import { useWordle } from '../context/WordleProvider';

const keyboardLayout = [
  ['q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'],
  ['a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l'],
  ['enter', 'z', 'x', 'c', 'v', 'b', 'n', 'm', 'backspace'],
] as const;

export const VirtualKeyboard = () => {
  const { keyboardHints, addLetter, removeLetter, submitGuess, isSending } =
    useWordle();

  useEffect(() => {
    const onKeyDown = (event: KeyboardEvent) => {
      if (event.metaKey || event.ctrlKey) return;
      const key = event.key.toLowerCase();

      if (key === 'enter') {
        event.preventDefault();
        submitGuess();
        return;
      }

      if (key === 'backspace') {
        event.preventDefault();
        removeLetter();
        return;
      }

      if (/^[a-z]$/.test(key)) {
        event.preventDefault();
        addLetter(key);
      }
    };

    window.addEventListener('keydown', onKeyDown);
    return () => window.removeEventListener('keydown', onKeyDown);
  }, [addLetter, removeLetter, submitGuess]);

  return (
    <div className="keyboard">
      {keyboardLayout.map((row, idx) => (
        <div className="keyboard-row" key={`kbd-row-${idx}`}>
          {row.map((key) => {
            const hint = keyboardHints[key] ?? 'unknown';
            const label =
              key === 'backspace' ? '⌫' : key === 'enter' ? 'Enter' : key;
            const handleClick = () => {
              if (key === 'enter') submitGuess();
              else if (key === 'backspace') removeLetter();
              else addLetter(key);
            };

            return (
              <button
                key={key}
                type="button"
                className={clsx('key', `key-${hint}`, {
                  'key-wide': key === 'enter' || key === 'backspace',
                  'is-disabled': isSending,
                })}
                onClick={handleClick}
                disabled={isSending}
              >
                {label.toUpperCase()}
              </button>
            );
          })}
        </div>
      ))}
    </div>
  );
};

