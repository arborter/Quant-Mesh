/*
This is the strategy layer.
Here is where the data produce from the market will
actually be utilized to make trades


*/

// This signal is the output of the strategy
typedef enum {
	SIGNAL_BUY = 1;
	SIGNAL_SELL = - 1;
	SIGNAL_HOLD = 0;
} TradeSignal;

// This struct is where each market tick lives
typedef struct {
	double price;
	int volume;
	double volatility;
	uint64_t time_stamp
} MarketData;

// This struct states the change of a stock over time
typedef struct {
	double last_price;       // historic data
	int last_volume;
	
	double ema_price;        // exponential moving average
	double ema_volatility;
	
	double variance;         // running variance estimate
	double stddev;
	
	uint64_t last_ts_ns;     // authoritative market time
	uint64_t tick_count;
} Stock_Delta;



int strategy_decide(const Position *f);
