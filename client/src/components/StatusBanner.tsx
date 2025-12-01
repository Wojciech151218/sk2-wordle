import { motion } from 'framer-motion';
import { useMemo } from 'react';
import { useWordle } from '../context/WordleProvider';

const bannerVariants = {
  initial: { opacity: 0, y: -20 },
  animate: { opacity: 1, y: 0, transition: { duration: 0.3 } },
};

export const StatusBanner = () => {
  const { connectionStatus, lastError, gameState, guesses, maxWords } =
    useWordle();

  const { tone, message, subtext } = useMemo(() => {
    if (lastError) {
      return {
        tone: 'danger',
        message: 'Something went wrong',
        subtext: lastError,
      };
    }

    if (connectionStatus === 'connecting') {
      return {
        tone: 'info',
        message: 'Connecting to Wordle server…',
        subtext: 'Preparing your board',
      };
    }

    if (connectionStatus === 'error') {
      return {
        tone: 'danger',
        message: 'Lost connection',
        subtext: 'Attempting to reconnect automatically',
      };
    }

    if (guesses.length >= maxWords) {
      return {
        tone: 'success',
        message: 'Game complete',
        subtext: 'No more guesses left',
      };
    }

    if (gameState?.message) {
      return {
        tone: 'info',
        message: gameState.message,
        subtext: null,
      };
    }

    return {
      tone: 'success',
      message: 'Connected',
      subtext: 'Make your best guess!',
    };
  }, [connectionStatus, gameState?.message, guesses.length, lastError, maxWords]);

  return (
    <motion.div
      className={`status-banner status-${tone}`}
      variants={bannerVariants}
      initial="initial"
      animate="animate"
    >
      <div>
        <p>{message}</p>
        {subtext ? <span>{subtext}</span> : null}
      </div>
      <span className="status-pill">{connectionStatus}</span>
    </motion.div>
  );
};

