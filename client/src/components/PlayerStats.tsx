/**
 * Player Stats - Displays stats for current player and others
 */

import React from 'react';
import type { Player } from '../types';

interface PlayerStatsProps {
  players: Player[];
  currentPlayerName: string;
  roundNumber: number;
}

export const PlayerStats: React.FC<PlayerStatsProps> = ({
  players,
  currentPlayerName,
  roundNumber,
}) => {
  // Sort players: current player first, then by alive status, then by errors
  const sortedPlayers = React.useMemo(() => {
    return [...players].sort((a, b) => {
      // Current player first
      if (a.player_name === currentPlayerName) return -1;
      if (b.player_name === currentPlayerName) return 1;

      // Alive players before eliminated
      if (a.is_alive !== b.is_alive) return a.is_alive ? -1 : 1;

      // Sort by errors (fewer errors first)
      return a.all_errors - b.all_errors;
    });
  }, [players, currentPlayerName]);

  return (
    <div className="player-stats">
      <div className="stats-header">
        <h3>Round {roundNumber}</h3>
        <p className="alive-count">
          {players.filter((p) => p.is_alive).length} / {players.length} alive
        </p>
      </div>

      <div className="stats-list">
        {sortedPlayers.map((player) => {
          const isCurrentPlayer = player.player_name === currentPlayerName;

          return (
            <div
              key={player.player_name}
              className={`stat-card ${isCurrentPlayer ? 'stat-card-current' : ''} ${
                !player.is_alive ? 'stat-card-eliminated' : ''
              }`}
            >
              <div className="stat-header">
                <span className="stat-name">
                  {player.player_name}
                  {isCurrentPlayer && ' (You)'}
                </span>
                <span className={`stat-status ${player.is_alive ? 'alive' : 'eliminated'}`}>
                  {player.is_alive ? '●' : '✕'}
                </span>
              </div>

              <div className="stat-details">
                <div className="stat-item">
                  <span className="stat-label">Round Errors:</span>
                  <span className="stat-value">{player.round_errors}</span>
                </div>
                <div className="stat-item">
                  <span className="stat-label">Total Errors:</span>
                  <span className="stat-value">{player.all_errors}</span>
                </div>
              </div>
            </div>
          );
        })}
      </div>
    </div>
  );
};

