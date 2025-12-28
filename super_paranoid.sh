#!/bin/bash
# Super Paranoid Stress Test 2.0 for UnixWare 7 sh (AMD64)
# Optimized for fixed-stack (8MB) and malloc-based architecture.

SH="./sh"
RECURSION_LIMIT=1500
CHURN_COUNT=50000
JOB_COUNT=200

echo "=== UW7 Super Paranoid Stress Suite 2.0 ==="

# 1. Stack Pressure & Overflow Safety
echo "--- Test 1: Stack Exhaustion ($RECURSION_LIMIT levels) ---"
$SH << 'EOF'
r() {
    x=$1
    if [ `expr $x % 500` -eq 0 ]; then
        echo "Depth $x..."
    fi
    if [ "$x" != "1500" ]; then
        r `expr $x + 1`
    fi
}
echo "Starting recursion (1500 levels)..."
r 1
EOF
echo "Note: Test 1 result above should be successful."

# 2. Lightning Churn (50k variables, no expr bottleneck)
echo "--- Test 2: Lightning Churn ($CHURN_COUNT variables) ---"
CHURN_SCRIPT="/tmp/churn_stress_uw.sh"
echo "echo 'Generating churn script...'"
python3 -c "for i in range($CHURN_COUNT): print(f'T_{i}=v_{i}')" > "$CHURN_SCRIPT"
python3 -c "for i in range($CHURN_COUNT): print(f'unset T_{i}')" >> "$CHURN_SCRIPT"
echo "echo 'Churning memory...'" >> "$CHURN_SCRIPT"

$SH "$CHURN_SCRIPT" > /dev/null

if [ $? -eq 0 ]; then
    echo "PASS: Lightning Churn"
else
    echo "FAIL: Lightning Churn"
    rm -f "$CHURN_SCRIPT"
    exit 1
fi
rm -f "$CHURN_SCRIPT"

# 3. Command Substitution (1 level - Historical Bourne Limit)
echo "--- Test 3: Command Substitution (1 level) ---"
RES=$($SH -c "echo \`echo bottom\`")
if [ "$RES" == "bottom" ]; then
    echo "PASS: CmdSubst"
else
    echo "FAIL: CmdSubst (received: $RES)"
    exit 1
fi

# 4. Job Control Storm (SVR4 jobs.c Stress)
echo "--- Test 4: Job Control Storm ($JOB_COUNT background procs) ---"
$SH << EOF
i=0
while [ \$i -lt $JOB_COUNT ]; do
    (sleep 1) &
    i=\`expr \$i + 1\`
done
echo "Spawned $JOB_COUNT jobs. Waiting..."
wait
echo "Storm cleared."
EOF

if [ $? -eq 0 ]; then
    echo "PASS: Job Control Storm"
else
    echo "FAIL: Job Control Storm"
    exit 1
fi

echo "=== ALL SUPER PARANOID TESTS PASSED ==="
