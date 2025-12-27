/**
 * Join Screen - First screen where users can see game state and join
 */

import React, { useState } from 'react';
import type { GameState } from '../types';
import { GameStateDisplay } from './GameStateDisplay';

interface JoinScreenProps {
  gameState: GameState | null;
  isConnected: boolean;
  onJoin: (playerName: string) => Promise<void>;
  loading: boolean;
  error: string | null;
}

export const JoinScreen: React.FC<JoinScreenProps> = ({
  gameState,
  isConnected,
  onJoin,
  loading,
  error,
}) => {
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
      await onJoin(playerName.trim());
    } catch (err) {
      // Error is handled by parent
    }
  };

  return (
    <div className="screen join-screen">
      <div className="screen-content">
        <header className="screen-header">
          <h1>⚔️ Wordle Battle Royale</h1>
          <p className="subtitle">Join the ultimate word-guessing showdown</p>
        </header>

        <GameStateDisplay gameState={gameState} isConnected={isConnected} />

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
                disabled={loading || !isConnected}
                maxLength={20}
                autoFocus
              />
            </div>

            {(error || localError) && (
              <div className="error-message">
                {error || localError}
              </div>
            )}

            <button
              type="submit"
              className="btn btn-primary"
              disabled={loading || !isConnected || !playerName.trim()}
            >
              {loading ? 'Joining...' : 'Join Game'}
            </button>
          </form>
        </div>
      </div>
    </div>
  );
};

