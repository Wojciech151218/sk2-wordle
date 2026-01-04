import React from 'react';
import type { Player } from '../types/api';

interface PlayersListProps {
  players: Player[];
  title: string;
  isGameRunning: boolean;
  enableKick?: boolean;
  currentPlayerName?: string | null;
  disableKick?: boolean;
  onKick?: (votedPlayer: string) => void;
}

const getInitials = (name: string): string => {
  const trimmed = name.trim();
  if (!trimmed) return '?';
  const parts = trimmed.split(/\s+/).filter(Boolean);
  if (parts.length === 1) return parts[0].slice(0, 1).toUpperCase();
  return (parts[0].slice(0, 1) + parts[1].slice(0, 1)).toUpperCase();
};

export const PlayersList: React.FC<PlayersListProps> = ({
  players,
  title,
  isGameRunning,
  enableKick = false,
  currentPlayerName,
  disableKick = false,
  onKick,
}) => {
  return (
    <div className="info-section">
      <h3>{title} ({players.length})</h3>
      <div className="players-list">
        {players.length === 0 ? (
          <p className="empty-state">No players yet</p>
        ) : (
          players.map((player) => (
            <div key={player.player_name} className="player-item">
              <div className="player-left">
                <div className="player-avatar">
                  <span className="player-initials">{getInitials(player.player_name)}</span>
                  {enableKick && onKick && player.player_name !== currentPlayerName && (
                    <button
                      type="button"
                      className="player-kick-btn"
                      onClick={(e) => {
                        e.stopPropagation();
                        onKick(player.player_name);
                      }}
                      disabled={disableKick}
                      title={`Start vote to kick ${player.player_name}`}
                      aria-label={`Start vote to kick ${player.player_name}`}
                    >
                      ✕
                    </button>
                  )}
                </div>
                <span className="player-name">{player.player_name}</span>
              </div>
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

