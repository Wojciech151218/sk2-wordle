/**
 * Lobby Screen - Shown when player is in lobby (game is null)
 */

import React from 'react';
import type { GameState } from '../types';
import { GameStateDisplay } from './GameStateDisplay';

interface LobbyScreenProps {
  gameState: GameState | null;
  isConnected: boolean;
  playerName: string;
  onReady: () => Promise<void>;
  onLeave: () => Promise<void>;
  loading: boolean;
  error: string | null;
}

export const LobbyScreen: React.FC<LobbyScreenProps> = ({
  gameState,
  isConnected,
  playerName,
  onReady,
  onLeave,
  loading,
  error,
}) => {
  const currentPlayer = gameState?.players_list.find(
    (p) => p.player_name === playerName
  );

  return (
    <div className="screen lobby-screen">
      <div className="screen-content">
        <header className="screen-header">
          <h1>⚔️ Wordle Battle Royale</h1>
          <p className="subtitle">Welcome, <strong>{playerName}</strong>!</p>
        </header>

        <GameStateDisplay gameState={gameState} isConnected={isConnected} />

        <div className="lobby-actions">
          <div className="player-status">
            {currentPlayer && (
              <div className={`status-badge ${currentPlayer.is_ready ? 'status-ready' : 'status-not-ready'}`}>
                {currentPlayer.is_ready ? '✓ You are ready!' : 'Not ready yet'}
              </div>
            )}
          </div>

          {error && (
            <div className="error-message">
              {error}
            </div>
          )}

          <div className="action-buttons">
            <button
              onClick={onReady}
              disabled={loading || !isConnected || currentPlayer?.is_ready}
              className="btn btn-primary btn-large"
            >
              {loading ? 'Processing...' : currentPlayer?.is_ready ? 'Ready!' : 'Ready Up'}
            </button>

            <button
              onClick={onLeave}
              disabled={loading || !isConnected}
              className="btn btn-secondary btn-large"
            >
              Leave Game
            </button>
          </div>

          <div className="lobby-info">
            <p>Waiting for all players to ready up...</p>
            <p className="hint">The game will start when everyone is ready</p>
          </div>
        </div>
      </div>
    </div>
  );
};

