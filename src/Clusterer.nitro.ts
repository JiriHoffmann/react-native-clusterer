import type { HybridObject } from 'react-native-nitro-modules';

export interface Clusterer
  extends HybridObject<{ ios: 'c++'; android: 'c++' }> {}
