Group 0:

- Testing R,G,B,A encoding
- Same constants for all stages (no locals)
- Using const0 and const1 in final-combiner


Group 1:

- Continuation of group 0
- Trusting R,G,B,A encoding, so fewer tests
- Using color in final-combiner, but forcing first stage to garbage
- Per stage constants (forced by locals)


Group 2:

- Similar to group 0 / 1
- Same constants for all stages (no locals)
- Using const0 and const1 in first stage


Group 3:

- Similar to group 2
- Per stage constants (forced by locals)


Group 4 / 5:

- Similar to group 2 / 3
- Using second stage instead
- Testing because first stage also had special meaning (for same-constant mode)


Group 6:

- Testing mode where each/same constant would differ


Group 7:

- Using a global-const in second stage, that is also local in first-stage

