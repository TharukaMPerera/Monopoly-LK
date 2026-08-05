// Player strategies
typedef enum {
    AGGRESSIVE_INVESTOR,
    CONSERVATIVE_BANKER,
    RISK_TAKER,
    BALANCED
} StrategyType;

// Insurance types
typedef enum {
    NONE,
    BASIC,
    COMPREHENSIVE,
    BUSINESS_INTERRUPTION
} InsuranceType;

// Building types
typedef enum {
    HOUSE,
    HOTEL
} BuildingType;

// Property struct
typedef struct {
    char name[50];
    int purchase_price;
    int mortgage_value;
    int base_rent;
    int house_cost;
    int hotel_cost;
    int num_buildings;      // 0–4 houses, 1 hotel
    int owner_id;           // -1 if owned by bank
    int is_mortgaged;
    int age;                // for depreciation
} Property;

// Loan struct
typedef struct {
    int active;             // 0 = no loan, 1 = active
    double amount;
    double interest_rate;
    int duration;           // rounds remaining
} Loan;

// Insurance struct
typedef struct {
    int active;             // 0 = none, 1 = active
    InsuranceType type;
    int rounds_remaining;
    double premium;
    double compensation;
} Insurance;

// Building struct
typedef struct {
    BuildingType type;
    double condition;       // 0–100%
} Building;

// Player struct
typedef struct {
    char name[30];
    int id;
    double cash;
    Property *owned_properties[40];   // pointers to properties
    int num_properties;
    Loan loan;
    Insurance policies[40];           // one per property
    Building buildings[40];           // condition tracking
    int position;                     // board index (0–39)
    int in_jail;                      // 0 = free, 1 = jailed
    StrategyType strategy;
    int bankrupt;                     // 0 = solvent, 1 = bankrupt
} Player;
