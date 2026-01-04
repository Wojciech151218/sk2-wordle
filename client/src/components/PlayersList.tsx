import React from 'react';
import type { Player } from '../types/api';

interface PlayersListProps {
  players: Player[];
  title: string;
  isGameRunning: boolean;
}

export const PlayersList: React.FC<PlayersListProps> = ({ players, title, isGameRunning }) => {
  return (
    <div className="info-section">
      <h3>{title} ({players.length})</h3>
      <div className="players-list">
        {players.length === 0 ? (
          <p className="empty-state">No players yet</p>
        ) : (
          players.map((player) => (
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
  );
};

