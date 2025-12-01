import { motion, type Variants } from 'framer-motion';
import { useMemo } from 'react';
import { useWordle } from '../context/WordleProvider';
import type { EvaluatedLetter, LetterState } from '../types/wordle';

const tileVariants: Variants = {
  hidden: { rotateX: -90, opacity: 0 },
  reveal: {
    rotateX: 0,
    opacity: 1,
    transition: { type: 'spring', stiffness: 200, damping: 20 },
  },
  bounce: {
    y: [0, -6, 0],
    transition: { duration: 0.24, ease: 'easeOut' },
  },
};

const buildEmptyRow = (lettersPerWord: number): EvaluatedLetter[] =>
  Array.from({ length: lettersPerWord }, () => ({
    letter: '',
    type: 'unknown' as LetterState,
  }));

export const GameBoard = () => {
  const { guesses, currentGuess, lettersPerWord, maxWords } = useWordle();

  const rows = useMemo(() => {
    const computed = guesses.map((word) => {
      if (word.letters.length === lettersPerWord) return word.letters;
      const padded = [...word.letters];
      while (padded.length < lettersPerWord) {
        padded.push({ letter: '', type: 'unknown' });
      }
      return padded;
    });

    if (computed.length < maxWords) {
      const activeRow = buildEmptyRow(lettersPerWord).map((slot, idx) => ({
        ...slot,
        letter: currentGuess[idx]?.toUpperCase() ?? '',
      }));
      computed.push(activeRow);
    }

    while (computed.length < maxWords) {
      computed.push(buildEmptyRow(lettersPerWord));
    }

    return computed;
  }, [currentGuess, guesses, lettersPerWord, maxWords]);

  return (
    <div 
      className="board" 
      style={{ 
        gridTemplateRows: `repeat(${maxWords}, minmax(0, 1fr))`
      }}
    >
      {rows.map((row, rowIndex) => (
        <div 
          className="board-row" 
          key={`row-${rowIndex}`}
          style={{ gridTemplateColumns: `repeat(${lettersPerWord}, minmax(0, 1fr))` }}
        >
          {row.map((tile, tileIndex) => {
            const stateClass = `tile-${tile.type}`;
            const isFinalized = rowIndex < guesses.length;
            const isEmpty = !tile.letter;
            const isCurrentRow = rowIndex === guesses.length;
            
            const animationState = isFinalized
              ? 'reveal'
              : 'bounce';

            return (
              <motion.div
                key={`tile-${rowIndex}-${tileIndex}`}
                className={`tile ${stateClass} ${isEmpty && isCurrentRow ? 'tile-empty-active' : ''}`}
                variants={tileVariants}
                initial="bounce"
                animate={animationState}
                transition={{ delay: tileIndex * 0.05 }}
              >
                <span>{tile.letter.toUpperCase()}</span>
              </motion.div>
            );
          })}
        </div>
      ))}
    </div>
  );
};

