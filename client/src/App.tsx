/**
 * Main App - Uses shared game context instead of prop drilling
 */

import './index.css';
import { JoinScreen } from './components/JoinScreen';
import { LobbyScreen } from './components/LobbyScreen';
import { GameScreen } from './components/GameScreen';
import { useGameContext } from './context';

function App() {
  const { currentScreen } = useGameContext();

  if (currentScreen === 'join') {
    return <JoinScreen />;
  }

  if (currentScreen === 'lobby') {
    return <LobbyScreen />;
  }

  if (currentScreen === 'game') {
    return <GameScreen />;
  }

  return null;
}

export default App;
