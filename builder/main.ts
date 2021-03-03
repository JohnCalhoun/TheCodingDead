import { writeFileSync } from "fs";

const NUM_OF_ROOMS = 100000
const NUM_OF_PEOPLE =1000000

interface VertexIface {
    id: string
    type: string
}

interface RoomIface extends VertexIface {
    id: string
    type: string
    width: number
    height: number
}

interface PersonIface extends VertexIface {
}

interface EdgeIface {
    type: string
}
interface DoorIface extends EdgeIface {
    roomA: string
    roomB: string
    roomAx: number
    roomAy: number
    roomBx: number
    roomBy: number
}

interface InIface extends EdgeIface {
    person: string
    room: string
}
var rooms: RoomIface[] = []
var people: PersonIface[] = []
var vertexs: string[] = []
var edges: string[] = []

for(var i=0; i <NUM_OF_ROOMS; i++){
    rooms.push({
        id:i.toString(),
        type:"room",
        width:10.0,
        height:10.0
    })
    edges.push(JSON.stringify({
        type:"door",
        roomA:i.toString(),
        roomB: i === NUM_OF_ROOMS -1 ? "0" : (i+1).toString(),
        roomAx: 10.0,
        roomAy: 5.0,
        roomBx: 0,
        roomBy: 5.1,
    }))
}

for(; i <NUM_OF_PEOPLE+NUM_OF_ROOMS; i++){
    people.push({
        id:i.toString(),
        type:"person",
    })
    edges.push(JSON.stringify({
        type:"in",
        person:i.toString(),
        room: rooms[Math.ceil(Math.random()*rooms.length)-1].id
    }))
}

rooms.forEach((x: RoomIface)=>vertexs.push(JSON.stringify(x)))
people.forEach((x: PersonIface)=>vertexs.push(JSON.stringify(x)))
writeFileSync("../data/vertex_1.csv", vertexs.join('\n')+'\n')
writeFileSync("../data/edge_1.csv", edges.join('\n') + '\n')


