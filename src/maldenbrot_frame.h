#ifndef MALDENBROT_FRAME_H
#define MALDENBROT_FRAME_h

bool maldenbrot_frame_intrin         (maldenbrot *const paint);
bool maldenbrot_frame_cycle_separated(maldenbrot *const paint);
bool maldenbrot_frame_cycle_all_in   (maldenbrot *const paint);
bool maldenbrot_frame_simple         (maldenbrot *const paint);

#endif //MALDENBROT_FRAME_H