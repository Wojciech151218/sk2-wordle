import React, { useState } from 'react';
import { GameStateDisplay } from './GameStateDisplay';
import { useGameContext } from '../context';

/**
 * Join Screen - First screen where users can see game state and join
 */
export const JoinScreen: React.FC = () => {
  const {
    gameState,
    connectionStatus,
    joinGame,
    apiLoading,
    errorMessage,
  } = useGameContext();
  const [playerName, setPlayerName] = useState('');
  const [localError, setLocalError] = useState<string | null>(null);

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setLocalError(null);

    if (!playerName.trim()) {
      setLocalError('Please enter a player name');
      return;
    }

    if (playerName.trim().length < 2) {
      setLocalError('Player name must be at least 2 characters');
      return;
    }

    try {
      await joinGame(playerName.trim());
    } catch {
      // Error handled by context state
    }
  };

  return (
    <div className="screen join-screen">
      <div className="screen-content">
        <header className="screen-header">
          <h1>⚔️ Wordle Battle Royale</h1>
          <p className="subtitle">Join the ultimate word-guessing showdown</p>
        </header>
        
      
        <div className="join-form-container">
          <h2>Join Game</h2>
          <form onSubmit={handleSubmit} className="join-form">
            <div className="form-group">
              <label htmlFor="playerName">Player Name</label>
              <input
                id="playerName"
                type="text"
                value={playerName}
                onChange={(e) => setPlayerName(e.target.value)}
                placeholder="Enter your name"
                maxLength={20}
                autoFocus
              />
            </div>

            {(errorMessage || localError) && (
              <div className="error-message">
                {errorMessage || localError}
              </div>
            )}

            <button
              type="submit"
              className="btn btn-primary"
            >
              {apiLoading ? 'Joining...' : 'Join Game'}
            </button>
          </form>
        </div>
      </div>
    </div>
  );
};