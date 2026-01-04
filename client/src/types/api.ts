/**
 * API Types for Wordle Game
 * Based on the backend API structure
 */

// ============================================================================
// Player Types
// ============================================================================

export interface Player {
  player_name: string;
  round_errors: number;
  all_errors: number;
  is_alive: boolean;
  is_ready: boolean;
}

// ============================================================================
// Round Types
// ============================================================================

export interface Round {
  word: string;
  round_end_time: number; // Unix timestamp
  round_duration: number; // Seconds
  round_start_time: number; // Unix timestamp
}

// ============================================================================
// Game Types
// ============================================================================

export interface Game {
  round_end_time: number;
  round_duration: number;
  game_start_time: number;
  players_list: Player[];
  rounds: Round[];
}

export interface Vote {
  voted_player: string;
  votes_for: string[];
  votes_against: string[];
}

// ============================================================================
// State Types
// ============================================================================

export interface GameState {
  round_end_time: number;
  round_duration: number;
  game_start_time: number;
  players_list: Player[]; // Empty when game is active
  game: Game | null; // null when in lobby, Game object when active
  current_vote: Vote | null; // null when no vote is active, Vote object when active
  vote_duration: number; // Seconds
  vote_end_time: number; // Unix timestamp (0 when no vote is active)
}

// ============================================================================
// Guess Types
// ============================================================================

export type LetterType = "green" | "yellow" | "gray";

export interface GuessLetter {
  letter: string;
  type: LetterType;
}

export interface GuessHistoryItem {
  letters: GuessLetter[];
}

export type GuessHistory = GuessHistoryItem[];

// ============================================================================
// Request Types
// ============================================================================

export interface JoinRequest {
  player_name: string;
}

export interface LeaveRequest {
  player_name: string;
}

export interface ReadyRequest {
  player_name: string;
  timestamp: string;
}

export interface StateRequest {
  player_name: string;
  timestamp: string;
}

export interface VoteRequest {
  voted_player: string;
  voting_player: string;
  vote_for: boolean;
}

export interface GuessRequest {
  player_name: string;
  timestamp: string;
  guess: string;
}

// ============================================================================
// Response Types
// ============================================================================

export interface JoinResponse extends GameState {}

export interface LeaveResponse extends GameState {}

export interface ReadyResponse extends GameState {}

export interface StateResponse extends GameState {}

export interface VoteResponse extends GameState {}

export interface GuessResponse {
  state: GameState;
  guess_result: GuessHistory;
}

// ============================================================================
// Utility Types
// ============================================================================

export interface ApiError {
  message: string;
  status?: number;
}

