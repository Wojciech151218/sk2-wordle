export type LetterState = 'gray' | 'yellow' | 'green' | 'unknown';

export type EvaluatedLetter = {
  letter: string;
  type: LetterState;
};

export type EvaluatedWord = {
  letters: EvaluatedLetter[];
};

export type ServerGameState = {
  letters_per_word: number;
  max_words: number;
  status: string;
  words: EvaluatedWord[];
  message?: string;
};

export type ConnectionStatus = 'idle' | 'connecting' | 'open' | 'closed' | 'error';

export type WordlePayload = ServerGameState;

export type ProxyEvent =
  | { type: 'tcp_connected' }
  | { type: 'tcp_closed' }
  | { type: 'error'; source: 'tcp' | 'proxy' | 'ws'; message: string };

export type IncomingMessage = WordlePayload | ProxyEvent;

