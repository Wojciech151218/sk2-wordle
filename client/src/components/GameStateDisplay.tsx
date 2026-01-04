import React from 'react';
import { useGameContext } from '../context';
import { PlayersList } from './PlayersList';

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
  const playersInLobby = gameState.players_list;
  const playersInGame = activeGame?.players_list ?? [];

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

        <PlayersList 
          players={playersInLobby} 
          title="Players in Lobby" 
          isGameRunning={isGameRunning} 
        />
        <PlayersList 
          players={playersInGame} 
          title="Players in Game" 
          isGameRunning={isGameRunning} 
        />
    </div>
  );
};

