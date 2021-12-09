const getRandomId = () =>{
  return `${Math.floor(Math.random() * Math.floor(Math.random() * Date.now()))}`
}

interface SuperclusterOptions {
  radius?: number,
  maxZoom?: number,
  minZoom?: number,
  extent?: number,
  minSize?: number,
}

// @ts-ignore
const module = global.clustererModule
export default class Clusterer {
  private id: string

  constructor(points: any, options?: SuperclusterOptions) {
    this.id = getRandomId()
    module.init(this.id, points, options);
  }

  getClusters(bbox:any, zoom: number): any {
    return null;
  }

  getTile(x: number, y: number, z: number): any {
    return module.getTile(this.id, x, y, z);
  }
  
  getChildren(clusterId: string): any {
    return module.getChildren(this.id, clusterId);
  }

  getLeaves(clusterId: string, limit?: number, offset?: number): any {
    return module.getLeaves(this.id, clusterId, limit ?? 10, offset ?? 0);
  }

  getClusterExpansionZoom(clusterId: string): number {
    return module.getClusterExpansionZoom(this.id, clusterId);
  }
  
}