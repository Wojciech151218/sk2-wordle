import React from 'react';
import { useGameContext } from '../context';

/**
 * Reusable component to display game state information
 * Used in both Join and Lobby screens
 */
export const GameStateDisplay: React.FC = () => {
  const { gameState, connectionStatus } = useGameContext();
  if (connectionStatus === 'disconnected') {
    return (
      <div className="state-display">
        <div className="status-badge status-disconnected">
          Disconnected from Server
        </div>
        <p className="status-message">Connecting to game server...</p>
      </div>
    );
  }

  if (!gameState) {
    return (
      <div className="state-display">
        <div className="status-badge status-loading">Loading...</div>
      </div>
    );
  }

  const activeGame = gameState.game;
  const isGameRunning = Boolean(activeGame);
  const playersInLobby = isGameRunning 
    ? activeGame!.players_list 
    : gameState.players_list;

  return (
    <div className="state-display">
      <div className={`status-badge ${isGameRunning ? 'status-running' : 'status-waiting'}`}>
        {isGameRunning ? 'Game In Progress' : 'Waiting in Lobby'}
      </div>

      <div className="info-section">
        <h3>Game Settings</h3>
        <div className="info-grid">
          <div className="info-item">
            <span className="info-label">Round Duration:</span>
            <span className="info-value">{gameState.round_duration}s</span>
          </div>
          {isGameRunning && gameState.game && (
            <>
              <div className="info-item">
                <span className="info-label">Total Rounds:</span>
                <span className="info-value">{gameState.game.rounds.length}</span>
              </div>
              <div className="info-item">
                <span className="info-label">Time Remaining:</span>
                <span className="info-value">
                  {Math.max(0, Math.floor(gameState.round_end_time - Date.now() / 1000))}s
                </span>
              </div>
            </>
          )}
        </div>
      </div>

      <div className="info-section">
        <h3>Players ({playersInLobby.length})</h3>
        <div className="players-list">
          {playersInLobby.length === 0 ? (
            <p className="empty-state">No players yet</p>
          ) : (
            playersInLobby.map((player) => (
              <div key={player.player_name} className="player-item">
                <span className="player-name">{player.player_name}</span>
                {!isGameRunning && (
                  <span className={`ready-badge ${player.is_ready ? 'ready' : 'not-ready'}`}>
                    {player.is_ready ? '✓ Ready' : 'Not Ready'}
                  </span>
                )}
                {isGameRunning && (
                  <span className={`alive-badge ${player.is_alive ? 'alive' : 'eliminated'}`}>
                    {player.is_alive ? '● Alive' : '✕ Eliminated'}
                  </span>
                )}
              </div>
            ))
          )}
        </div>
      </div>
    </div>
  );
};

