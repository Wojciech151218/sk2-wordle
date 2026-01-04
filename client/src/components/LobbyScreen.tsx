import React from 'react';
import { GameStateDisplay } from './GameStateDisplay';
import { useGameContext } from '../context';

/**
 * Lobby Screen - Shown when player is in lobby (game is null)
 */
export const LobbyScreen: React.FC = () => {
  const {
    gameState,
    connectionStatus,
    playerName,
    readyUp,
    leaveGame,
    apiLoading,
    errorMessage,
  } = useGameContext();

  const safePlayerName = playerName ?? '';
  const currentPlayer = gameState?.players_list.find((p) => p.player_name === safePlayerName);

  return (
    <div className="screen lobby-screen">
      <div className="screen-content">
        <header className="screen-header">
          <h1>Wordle Battle Royale</h1>
          <p className="subtitle">Welcome, <strong>{safePlayerName}</strong>!</p>
        </header>

        <GameStateDisplay />

        <div className="lobby-actions">
          <div className="player-status">
            {currentPlayer && (
              <div className={`status-badge ${currentPlayer.is_ready ? 'status-ready' : 'status-not-ready'}`}>
                {currentPlayer.is_ready ? '✓ You are ready!' : 'Not ready yet'}
              </div>
            )}
          </div>

          {errorMessage && (
            <div className="error-message">
              {errorMessage}
            </div>
          )}

          <div className="action-buttons">
            <button
              onClick={readyUp}
              disabled={
                apiLoading ||
                connectionStatus !== 'connected' ||
                currentPlayer?.is_ready
              }
              className="btn btn-primary btn-large"
            >
              {apiLoading ? 'Processing...' : currentPlayer?.is_ready ? 'Ready!' : 'Ready Up'}
            </button>

            <button
              onClick={leaveGame}
              disabled={apiLoading || connectionStatus !== 'connected'}
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

