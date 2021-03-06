#include "ed_bsp.h"

//struct bsh_polygon_t *bsh_CopyPolygons(struct bsh_polygon_t *polygons)
//{
//    struct bsh_polygon_t *copy = NULL;
//    struct bsh_polygon_t *new_polygon = NULL;
//    struct bsh_polygon_t *prev_polygon = NULL;
//    
//    while(polygons)
//    {
//        new_polygon = mem_Calloc(1, sizeof(struct bsh_polygon_t));
//        memcpy(new_polygon, polygons, sizeof(struct bsh_polygon_t));
//        new_polygon->vertices = mem_Calloc(new_polygon->vert_count, sizeof(struct bsh_vertex_t));
//        memcpy(new_polygon->vertices, polygons->vertices, sizeof(struct bsh_vertex_t) * new_polygon->vert_count);
//        
//        if(!copy)
//        {
//            copy = new_polygon;
//        }
//        
//        if(prev_polygon)
//        {
//            prev_polygon->next = new_polygon;
//        }
//        
//        prev_polygon = new_polygon;
//        polygons = polygons->next;
//    }
//    
//    return copy;
//}
//
//void bsh_DestroyPolygons(struct bsh_polygon_t *polygons)
//{
//    struct bsh_polygon_t *next;
//    
//    while(polygons)
//    {
//        next = polygons->next;
//        mem_Free(polygons->vertices);
//        mem_Free(polygons);
//        polygons = next;
//    }
//}
//
//struct bsh_polygon_t *bsh_FlipPolygons(struct bsh_polygon_t *polygons)
//{
//    struct bsh_polygon_t *polygon = polygons;
//    struct bsh_vertex_t temp;
//    while(polygon)
//    {
//        for(uint32_t vert_index = 0; vert_index < polygon->vert_count >> 1; vert_index++)
//        {
//            uint32_t other_vert_index = polygon->vert_count - vert_index - 1;
//            temp = polygon->vertices[vert_index];
//            polygon->vertices[vert_index] = polygon->vertices[other_vert_index];
//            polygon->vertices[other_vert_index] = temp;
//        }
//        
//        vec3_t_mul(&polygon->face_normal, &polygon->face_normal, -1.0);
//        polygon = polygon->next;
//    }
//    
//    return polygons;
//}
//
//uint32_t bsh_SplitEdges(struct bsh_polygon_t *polygon, vec3_t *point, vec3_t *normal, uint32_t *edges, float *times)
//{
//    uint32_t edge_vert_index = 0;
//    float dists[2];
//    float denom;
//    vec3_t plane_edge_vec;
//    
//    for(uint32_t vert_index = 0; vert_index < polygon->vert_count;)
//    {
//        uint32_t first_edge_vert = vert_index;
//        
//        vec3_t_sub(&plane_edge_vec, &polygon->vertices[vert_index % polygon->vert_count].position, point);
//        dists[0] = vec3_t_dot(normal, &plane_edge_vec);
//        vert_index++;
//        vec3_t_sub(&plane_edge_vec, &polygon->vertices[vert_index % polygon->vert_count].position, point);
//        dists[1] = vec3_t_dot(normal, &plane_edge_vec);
//        
//        if(dists[0] * dists[1] <= 0.0)
//        {
//            denom = dists[1] - dists[0];
//            if(denom)
//            {
//                denom = fabs(dists[0] / denom);
//                
//                if(denom < 1.0)
//                {
//                    edges[edge_vert_index] = first_edge_vert;
//                    times[edge_vert_index] = denom;
//                    edge_vert_index++;
//                    
//                    if(edge_vert_index > 1)
//                    {
//                        break;
//                    }
//                }
//            }
//        }
//    }
//    
//    return edge_vert_index > 1;
//}
//
//uint32_t bsh_SplitPolygon(struct bsh_polygon_t *polygon, vec3_t *point, vec3_t *normal, struct bsh_polygon_t **front, struct bsh_polygon_t **back)
//{
//    uint32_t edges[2];
//    float times[2];
//    
//    struct bsh_polygon_t *splits[2] = {NULL};
//    struct bsh_polygon_t *split;
//
//    if(bsh_SplitEdges(polygon, point, normal, edges, times))
//    {
//        uint32_t split_verts;
//        
//        if(edges[0] > edges[1])
//        {
//            split_verts = (edges[1] + polygon->vert_count) - edges[0];
//        }
//        else
//        {
//            split_verts = edges[1] - edges[0];
//        }
//        
//        uint32_t verts_left = polygon->vert_count - split_verts;
//        
//        for(uint32_t split_index = 0; split_index < 2; split_index++)
//        {
//            split = mem_Calloc(1, sizeof(struct bsh_polygon_t));
//            split->face_normal = polygon->face_normal;
//            split->vertices = mem_Calloc(split_verts + 2, sizeof(struct bsh_vertex_t));
//            
//            uint32_t vert_index = edges[split_index];
//            uint32_t split_side = 0;
//            
//            struct bsh_vertex_t *vertex0 = polygon->vertices + edges[split_index];
//            struct bsh_vertex_t *vertex1 = polygon->vertices + (edges[split_index] + 1) % polygon->vert_count;
//            float time = times[split_index];
//            
//            vec3_t_lerp(&split->vertices[0].position, &vertex0->position, &vertex1->position, time);
//            vec3_t_lerp(&split->vertices[0].normal, &vertex0->normal, &vertex1->normal, time);
//            vec3_t_lerp(&split->vertices[0].tangent, &vertex0->tangent, &vertex1->tangent, time);
//            split->vert_count++;
//            
//            while(vert_index != edges[split_index ^ 1])
//            {
//                vert_index = (vert_index + 1) % polygon->vert_count;
//                split->vertices[split->vert_count] = polygon->vertices[vert_index];
//                split->vert_count++;
//                vec3_t plane_to_vec_vert;
//                vec3_t_sub(&plane_to_vec_vert, &polygon->vertices[vert_index].position, point);
//                /* find out which split is this. If the dot product between the vec from the plane to
//                the current vertex and the normal is negative, we'll be dealing with the back split.
//                Front split will be at index 0, back split will be at 1. */
//                split_side |= vec3_t_dot(&plane_to_vec_vert, normal) < 0.0;
//            }
//            
//            vertex0 = polygon->vertices + edges[split_index ^ 1];
//            vertex1 = polygon->vertices + (edges[split_index ^ 1] + 1) % polygon->vert_count;
//            time = times[split_index ^ 1];
//            
//            vec3_t_lerp(&split->vertices[split->vert_count].position, &vertex0->position, &vertex1->position, time);
//            vec3_t_lerp(&split->vertices[split->vert_count].normal, &vertex0->normal, &vertex1->normal, time);
//            vec3_t_lerp(&split->vertices[split->vert_count].tangent, &vertex0->tangent, &vertex1->tangent, time);
//            split->vert_count++;
//            
//            splits[split_side] = split;
//            split_verts = verts_left;
//        }
//        
//        *front = splits[0];
//        *back = splits[1];
//        
//        return 1;
//    }
//    
//    return 0;
//}
//
//void bsh_SplitPolygons(struct bsh_polygon_t *polygons, vec3_t *point, vec3_t *normal, struct bsh_polygon_t **front, struct bsh_polygon_t **back, struct bsh_polygon_t **coplanar)
//{
//    struct bsh_polygon_t *polygon;
//    struct bsh_polygon_t *next_polygon;
//    struct bsh_polygon_t *front_list = NULL;
//    struct bsh_polygon_t *back_list = NULL;
//    struct bsh_polygon_t *coplanar_list = NULL;
//    struct bsh_polygon_t *front_split;
//    struct bsh_polygon_t *back_split;
//    
//    polygon = polygons;
//    
//    while(polygon)
//    {
//        next_polygon = polygon->next;
//        
//        switch(bsh_ClassifyPolygon(polygon, point, normal))
//        {
//            case BSH_POLYGON_FRONT:
//                polygon->next = front_list;
//                front_list = polygon;
//            break;
//            
//            case BSH_POLYGON_BACK:
//                polygon->next = back_list;
//                back_list = polygon;
//            break;
//            
//            case BSH_POLYGON_COPLANAR:
//                polygon->next = coplanar_list;
//                coplanar_list = polygon;
//            break;
//            
//            case BSH_POLYGON_STRADDLING:
//                bsh_SplitPolygon(polygon, point, normal, &front_split, &back_split);
//                
//                polygon->next = NULL;
//                bsh_DestroyPolygons(polygon);
//                
//                front_split->next = front_list;
//                front_list = front_split;
//                
//                back_split->next = back_list;
//                front_list = back_split;
//            break;
//        }
//        
//        polygon = next_polygon;
//    }
//    
//    *front = front_list;
//    *back = back_list;
//    *coplanar = coplanar_list;
//}
//
//struct bsh_polygon_t *bsh_BestSplitter(struct bsh_polygon_t **polygons)
//{
//    struct bsh_polygon_t *splitter;
//    struct bsh_polygon_t *prev_splitter = NULL;
//    struct bsh_polygon_t *polygon;
//    struct bsh_polygon_t *best_splitter = NULL;
//    struct bsh_polygon_t *best_splitter_prev = NULL;
//    uint32_t best_split_count = 0xffffffff;
//    uint32_t cur_split_count;
//    uint32_t edges[2];
//    float times[2];
//    
//    splitter = *polygons;
//    
//    while(splitter)
//    {
//        polygon = splitter->next;
//        cur_split_count = 0;
//        while(polygon)
//        {
//            if(bsh_SplitEdges(polygon, &splitter->vertices[0].position, &splitter->face_normal, edges, times))
//            {
//                cur_split_count++;
//            }
//            polygon = polygon->next;
//        }
//        
//        if(cur_split_count < best_split_count)
//        {
//            best_split_count = cur_split_count;
//            best_splitter = splitter;
//            best_splitter_prev = prev_splitter;
//        }
//        
//        prev_splitter = splitter;
//        splitter = splitter->next;
//    }
//    
//    if(best_splitter_prev)
//    {
//        best_splitter_prev->next = best_splitter->next;
//    }
//    else
//    {
//        /* polygon is the first in the list, so update the list pointer */
//        *polygons = best_splitter->next;
//    }
//    
//    best_splitter->next = NULL;
//    
//    return best_splitter;
//}
//
//uint32_t bsh_ClassifyPoint(vec3_t *point, vec3_t *plane_point, vec3_t *plane_normal)
//{
//    vec3_t plane_point_vec;
//    float dist;
//    vec3_t_sub(&plane_point_vec, point, plane_point);
//    dist = vec3_t_dot(&plane_point_vec, plane_normal);
//    
//    if(dist > 0.0)
//    {
//        return BSH_POINT_FRONT;
//    }
//    else if(dist < 0.0)
//    {
//        return BSH_POINT_BACK;
//    }
//    
//    return BSH_POINT_COPLANAR;
//}
//
//uint32_t bsh_ClassifyPolygon(struct bsh_polygon_t *polygon, vec3_t *point, vec3_t *normal)
//{
//    int32_t last_classification = BSH_POINT_COPLANAR;
//    
//    for(uint32_t vert_index = 0; vert_index < polygon->vert_count; vert_index++)
//    {
//        int32_t classification = bsh_ClassifyPoint(&polygon->vertices[vert_index].position, point, normal);
//        
//        if(classification != last_classification)
//        {
//            if(abs(classification - last_classification) == 1)
//            {
//                /* if one is BSH_POINT_FRONT and the other is BSH_POINT_BACK */
//                return BSH_POLYGON_STRADDLING;
//            }
//            
//            if(classification != BSH_POINT_COPLANAR)
//            {
//                last_classification = classification;
//            }
//        }
//    }
//    
//    return last_classification;
//}
//
//struct bsh_bsp_t *bsh_BspFromPolygonsRecursive(struct bsh_polygon_t *polygons)
//{
//    struct bsh_polygon_t *splitter;
//    struct bsh_polygon_t *next_polygon;
//    struct bsh_polygon_t *front = NULL;
//    struct bsh_polygon_t *back = NULL; 
//    struct bsh_polygon_t *coplanar = NULL;
////    struct bsh_polygon_t *anti_coplanar = NULL;
//    struct bsh_bsp_t *bsp;
//    
//    bsp = mem_Calloc(1, sizeof(struct bsh_bsp_t));
//    splitter = bsh_BestSplitter(&polygons);
//    
//    if(polygons)
//    {
//        bsh_SplitPolygons(polygons, &splitter->vertices[0].position, &splitter->face_normal, &front, &back, &coplanar);
//    }
//    
//    bsp->splitter = splitter;
//    splitter->next = coplanar;
//    
//    if(front)
//    {
//        bsp->front = bsh_BspFromPolygonsRecursive(front);
//    }
//    else
//    {
//        bsp->front = mem_Calloc(1, sizeof(struct bsh_bsp_t));
//        bsp->front->type = BSH_BSP_TYPE_EMPTY;
//    }
//    
//    if(back)
//    {
//        bsp->back = bsh_BspFromPolygonsRecursive(back);
//    }
//    else
//    {
//        bsp->back = mem_Calloc(1, sizeof(struct bsh_bsp_t));
//        bsp->back->type = BSH_BSP_TYPE_SOLID;
//    }
//    
//    return bsp;
//}
//
//struct bsh_bsp_t *bsh_BspFromPolygons(struct bsh_polygon_t *polygons)
//{
//    struct bsh_polygon_t *polygons_copy = bsh_CopyPolygons(polygons);
//    return bsh_BspFromPolygonsRecursive(polygons_copy);
//}
//
//void bsh_DestroyBsp(struct bsh_bsp_t *bsp)
//{
//    if(bsp)
//    {
//        bsh_DestroyPolygons(bsp->splitter);
//        bsh_DestroyBsp(bsp->back);
//        bsh_DestroyBsp(bsp->front);
//        mem_Free(bsp);
//    }
//}
//
//struct bsh_bsp_t *bsh_IncrementalSetOpRecursive(struct bsh_bsp_t *bsp, struct bsh_polygon_t *polygons, uint32_t op)
//{
//    
//    struct bsh_polygon_t *front;
//    struct bsh_polygon_t *back;
//    struct bsh_polygon_t *coplanar;
//    
//    if(op == BSH_SET_OP_SUBTRACTION)
//    {
//        polygons = bsh_FlipPolygons(polygons);
//        op = BSH_SET_OP_INTERSECTION;
//    }
//    
//    if(bsp->back == bsp->front)
//    {
//        switch(op)
//        {
//            case BSH_SET_OP_UNION:
//                if(bsp->type == BSH_BSP_TYPE_EMPTY)
//                {
//                    return bsh_BspFromPolygonsRecursive(polygons);
//                }
//            break;
//            
//            case BSH_SET_OP_INTERSECTION:
//                if(bsp->type == BSH_BSP_TYPE_SOLID)
//                {
//                    return bsh_BspFromPolygonsRecursive(polygons);
//                }
//            break;
//        }
//        
//        return bsp;
//    }
//    else
//    {
//        
//    }
//}
//
//struct bsh_bsp_t *bsh_IncrementalSetOp(struct bsh_bsp_t *bsp, struct bsh_polygon_t *polygons, uint32_t op)
//{
//    struct bsh_polygon_t *polygons_copy = bsh_CopyPolygons(polygons);
//    return bsh_IncrementalSetOpRecursive(bsp, polygons_copy, op);
//}
//
