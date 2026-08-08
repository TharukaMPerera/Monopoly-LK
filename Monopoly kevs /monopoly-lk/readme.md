# MONOPOLY-LK Simulation

## SCS 1301 - Data Structures and Program Design using C
### University of Colombo School of Computing

A complete C implementation of the MONOPOLY-LK board game simulation featuring four autonomous AI players with distinct investment strategies.

## Quick Start

### Compile
```bash
gcc *.c -o monopoly
```

### Run
```bash
./monopoly        # Linux/macOS
monopoly.exe      # Windows
```

### Requirements
- GCC compiler (or any C99-compatible compiler)
- Standard C libraries only (stdio, stdlib, string, time)

## Project Structure

| File | Purpose |
|------|---------|
| `types.h` | All structures, enumerations, constants, and function declarations |
| `main.c` | Program entry point |
| `game.c` | Game controller, simulation engine, turn management, output formatting |
| `board.c` | Board initialization, player movement, landing resolution for all square types |
| `players.c` | AI decision-making algorithms for all four player strategies |
| `finance.c` | Loans, banking, insurance, depreciation, taxation, auctions, bankruptcy |
| `events.c` | Economic events, government regulations, national cards, regional cards, disasters, market reviews, inflation |

## Game Features

### Board (40 Squares)
- 22 properties across 8 colour groups (Brown through Dark Blue)
- 4 railway stations
- 2 utility companies
- 1 commercial bank (Bank of Ceylon)
- 2 insurance companies
- 3 National Event Card squares
- Income Tax, Jail, Go To Jail, Free Parking, GO

### Four Autonomous AI Players
1. **Aggressive Investor** - Maximizes long-term wealth through rapid expansion
2. **Conservative Banker** - Prioritizes financial stability, minimizes risk
3. **Risk Taker** - Speculative investments, willing to incur significant debt
4. **Opportunistic Trader** - Adapts dynamically to market conditions

### MONOPOLY-LK Extensions
- **Banking System** - Secured loans with collateral, interest accumulation, foreclosure
- **Insurance** - 3 policy types (Basic, Comprehensive, Business Interruption)
- **Inflation** - Compounding rate changes every 10 rounds (-3% to +12%)
- **Property Depreciation** - Age-based value loss with renovation option
- **Building Depreciation** - Condition rating affecting rental income
- **Economic Events** - 8 national-level events every 15 rounds
- **Government Regulations** - 8 policy changes every 20 rounds
- **Regional Development Cards** - 12 region-specific effects every 15 rounds
- **Dynamic Property Market** - Market booms and declines for property groups
- **National Event Card Deck** - 20 cards with various effects
- **Disasters** - Random disasters every 10 rounds

### Output
The simulation prints detailed messages for every significant event:
- Player movement and dice rolls
- Property purchases, rent payments
- Building and hotel construction
- Loan transactions and defaults
- Insurance purchases and claims
- Auction proceedings
- Economic events and government regulations
- End-of-round summaries with net worth
- Market conditions report
- Final game results

## Simulation Rules
- Maximum 500 rounds
- Game ends when only one player remains solvent, or after 500 rounds
- Winner determined by highest net worth
- No user interaction required after launch

## Modifying the Game

### Change Property Values
Edit the `prop_data` array in `board.c`.

### Change Player Strategies
Edit the strategy-specific functions in `players.c` (e.g., `aggressive_buy_property`, `conservative_develop`).

### Add New Events
Extend the `EconomicEventType` enum in `types.h`, add handling in `apply_economic_event` in `events.c`.

### Tune Simulation Parameters
Key constants in `types.h`:
- `STARTING_CASH` (default: 30000)
- `MAX_ROUNDS` (default: 500)
- `GO_SALARY` (default: 2000)
- `LOAN_DURATION` (default: 20)
- `INSURANCE_DURATION` (default: 20)

## Quality Coverage
- ✅ All 40 board squares mapped correctly
- ✅ All 8 property groups with correct pricing from assignment
- ✅ 4 railway stations with tiered rent (250/500/1000/2000)
- ✅ 2 utility companies with dice-based rent (4× / 10×)
- ✅ All 3 insurance types with premiums and coverage
- ✅ Loans with collateral, interest accumulation, default/foreclosure
- ✅ Property depreciation after 50 rounds, renovation option
- ✅ Building condition degradation (2% per round), maintenance
- ✅ Inflation compounding applied to all prices
- ✅ 8 economic events with proper effects
- ✅ 8 government regulations
- ✅ 12 regional development cards
- ✅ 20 national event cards
- ✅ 5 disaster types with insurance claims
- ✅ Auction system with strategy-based bidding
- ✅ Bankruptcy handling with asset liquidation
- ✅ Detailed output messages matching assignment specification
- ✅ End-of-round summaries with net worth breakdown
- ✅ Market conditions display at end of each round
