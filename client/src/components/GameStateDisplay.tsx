import React, { useEffect, useState } from 'react';
import { useGameContext } from '../context';
import { PlayersList } from './PlayersList';
import type { Vote } from '../types';

const hasVoted = (vote: Vote, playerName: string): boolean => {
  return vote.votes_for.includes(playerName) || vote.votes_against.includes(playerName);
};

/**
 * Reusable component to display game state information
 * Used in both Join and Lobby screens
 */
export const GameStateDisplay: React.FC = () => {
  const { gameState, connectionStatus, playerName, apiLoading, castVote } = useGameContext();
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
  const currentVote = gameState.current_vote;
  const [nowSec, setNowSec] = useState(() => Date.now() / 1000);

  // Locally tick the countdown while a vote is active so the UI updates even
  // if the server doesn't push state updates every second.
  useEffect(() => {
    if (!currentVote || !gameState.vote_end_time) return;

    setNowSec(Date.now() / 1000);
    const id = window.setInterval(() => {
      setNowSec(Date.now() / 1000);
    }, 250);

    return () => window.clearInterval(id);
  }, [currentVote, gameState.vote_end_time]);

  const voteTimeRemaining =
    currentVote && gameState.vote_end_time
      ? Math.max(0, Math.floor(gameState.vote_end_time - nowSec))
      : null;

  const kickEnabled = !currentVote && connectionStatus === 'connected' && Boolean(playerName);
  const disableKick = apiLoading || connectionStatus !== 'connected';

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

      {currentVote && (
        <div className="vote-modal-overlay" role="dialog" aria-modal="true" aria-label="Kick vote">
          <div className="vote-modal">
            <div className="vote-modal-header">
              <div className="vote-modal-title">
                Vote to kick <strong>{currentVote.voted_player}</strong>
              </div>
              <div className="vote-modal-subtitle">
                {voteTimeRemaining !== null && (
                  <>
                    Time remaining: <strong>{voteTimeRemaining}s</strong>
                    <span> · </span>
                  </>
                )}
                {playerName && (
                  <>
                    {playerName === currentVote.voted_player
                      ? "You can't vote in your own kick vote"
                      : hasVoted(currentVote, playerName)
                        ? 'You already voted'
                        : 'Cast your vote'}
                  </>
                )}
              </div>
            </div>

            <div className="vote-modal-actions">
              <button
                type="button"
                className="btn btn-primary"
                disabled={
                  apiLoading ||
                  connectionStatus !== 'connected' ||
                  !playerName ||
                  playerName === currentVote.voted_player ||
                  hasVoted(currentVote, playerName)
                }
                onClick={() => castVote(currentVote.voted_player, true)}
              >
                Vote kick
              </button>
              <button
                type="button"
                className="btn btn-secondary"
                disabled={
                  apiLoading ||
                  connectionStatus !== 'connected' ||
                  !playerName ||
                  playerName === currentVote.voted_player ||
                  hasVoted(currentVote, playerName)
                }
                onClick={() => castVote(currentVote.voted_player, false)}
              >
                Vote keep
              </button>
            </div>

            <div className="vote-modal-lists">
              <div className="vote-list vote-list-for">
                <div className="vote-list-header">
                  <span>For kick</span>
                  <span className="vote-count">{currentVote.votes_for.length}</span>
                </div>
                <div className="vote-list-body">
                  {currentVote.votes_for.length === 0 ? (
                    <div className="vote-empty">No votes yet</div>
                  ) : (
                    currentVote.votes_for.map((p) => (
                      <div key={`for-${p}`} className="vote-name">{p}</div>
                    ))
                  )}
                </div>
              </div>

              <div className="vote-list vote-list-against">
                <div className="vote-list-header">
                  <span>Against kick</span>
                  <span className="vote-count">{currentVote.votes_against.length}</span>
                </div>
                <div className="vote-list-body">
                  {currentVote.votes_against.length === 0 ? (
                    <div className="vote-empty">No votes yet</div>
                  ) : (
                    currentVote.votes_against.map((p) => (
                      <div key={`against-${p}`} className="vote-name">{p}</div>
                    ))
                  )}
                </div>
              </div>
            </div>
          </div>
        </div>
      )}

      <PlayersList
        players={playersInLobby}
        title="Players in Lobby"
        isGameRunning={isGameRunning}
        enableKick={kickEnabled}
        currentPlayerName={playerName}
        disableKick={disableKick}
        onKick={(votedPlayer) => castVote(votedPlayer, true)}
      />
      <PlayersList
        players={playersInGame}
        title="Players in Game"
        isGameRunning={isGameRunning}
        enableKick={kickEnabled}
        currentPlayerName={playerName}
        disableKick={disableKick}
        onKick={(votedPlayer) => castVote(votedPlayer, true)}
      />
    </div>
  );
};

