import { WordleProvider } from './context/WordleProvider';
import { GameBoard } from './components/GameBoard';
import { VirtualKeyboard } from './components/VirtualKeyboard';
import { StatusBanner } from './components/StatusBanner';

function App() {
  return (
    <WordleProvider>
      <div className="app-shell">
        <header className="app-header">
          <div className="glow-orb" />
          <h1>Wordle Live</h1>
        </header>
        <main className="app-content">
          <StatusBanner />
          <GameBoard />
          <VirtualKeyboard />
        </main>
      </div>
    </WordleProvider>
  );
}

export default App;
