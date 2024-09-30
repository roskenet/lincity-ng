/* ---------------------------------------------------------------------- *
 * src/lincity/modules/organic_farm.h
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2024 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#define GROUP_ORGANIC_FARM_COLOUR (green(30))
#define GROUP_ORGANIC_FARM_COST 1000
#define GROUP_ORGANIC_FARM_COST_MUL 20
#define GROUP_ORGANIC_FARM_BUL_COST 100
#define GROUP_ORGANIC_FARM_TECH 0
#define GROUP_ORGANIC_FARM_FIREC 20
#define GROUP_ORGANIC_FARM_RANGE 0
#define GROUP_ORGANIC_FARM_SIZE 4

#define ORGANIC_FARM_FOOD_OUTPUT 550
#define ORG_FARM_POWER_REC 50
#define ORG_FARM_WASTE_GET 6
#define FARM_LABOR_USED 13
#define WATER_FARM 50
#define FARM_WATER_GET (16 * WATER_FARM)
#define MAX_WATER_AT_FARM (20*FARM_WATER_GET)

#define MAX_ORG_FARM_FOOD  (ORGANIC_FARM_FOOD_OUTPUT * 20)
#define MAX_ORG_FARM_POWER (ORG_FARM_POWER_REC * 20)
#define MAX_ORG_FARM_WASTE (ORG_FARM_WASTE_GET * 20)
#define MAX_FARM_LABOR (FARM_LABOR_USED * 20)



/* gets waste only when powered */

#define MIN_FOOD_SOLD_FOR_ANIM 200


#include <stdlib.h>                 // for rand
#include <array>                    // for array
#include <string>                   // for basic_string

#include "modules.h"


class Organic_farmConstructionGroup: public ConstructionGroup {
public:
    Organic_farmConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range, 2/*mps_pages*/
    )
    {
        commodityRuleCount[STUFF_FOOD].maxload = MAX_ORG_FARM_FOOD;
        commodityRuleCount[STUFF_FOOD].take = false;
        commodityRuleCount[STUFF_FOOD].give = true;
        commodityRuleCount[STUFF_LOVOLT].maxload = MAX_ORG_FARM_POWER;
        commodityRuleCount[STUFF_LOVOLT].take = true;
        commodityRuleCount[STUFF_LOVOLT].give = false;
        commodityRuleCount[STUFF_LABOR].maxload = MAX_FARM_LABOR;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_WASTE].maxload = MAX_ORG_FARM_WASTE;
        commodityRuleCount[STUFF_WASTE].take = true;
        commodityRuleCount[STUFF_WASTE].give = false;
        commodityRuleCount[STUFF_WATER].maxload = MAX_WATER_AT_FARM;
        commodityRuleCount[STUFF_WATER].give = false;
        commodityRuleCount[STUFF_WATER].take = true;
    }
    // overriding method that creates a organic_farm
    virtual Construction *createConstruction(int x, int y);
};

extern Organic_farmConstructionGroup organic_farmConstructionGroup;

class Organic_farm: public RegisteredConstruction<Organic_farm> { // Organic_farm inherits from its own RegisteredConstruction
public:
    Organic_farm(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Organic_farm>(x, y)
    {
        this->constructionGroup = cstgrp;
        init_resources();
        this->tech = tech_level;
        setMemberSaved(&this->tech, "tech");
        // this->tech_bonus = int( ((long long int)tech_level * ORGANIC_FARM_FOOD_OUTPUT) / MAX_TECH_LEVEL );
        setMemberSaved(&this->tech_bonus, "tech_bonus"); // compatibility
        this->crop_rotation_key = (rand() % 4) + 1;
        this->month_stagger = rand() % 100;
        this->food_this_month = 0;
        this->food_last_month = 0;
        //this->max_foodprod = 0;
        initialize_commodities();
        // Check underground water, and reduce food production accordingly
        int w = 0;
        if (use_waterwell)
        {
            for (int i = 0; i < constructionGroup->size; i++)
            {
                for (int j = 0; j < constructionGroup->size; j++)
                {
                    if (world(x + j, y + i)->flags & FLAG_HAS_UNDERGROUND_WATER)
                        w++;
                }// end j
            }//end i
            this->ugwCount = w;
        }
        else //no waterwell
        {
            this->ugwCount = 16;
        }

        commodityMaxCons[STUFF_WASTE] = 100 * ORG_FARM_WASTE_GET;
        commodityMaxCons[STUFF_LABOR] = 100 * FARM_LABOR_USED;
        commodityMaxCons[STUFF_LOVOLT] = 100 * ORG_FARM_POWER_REC;
        commodityMaxCons[STUFF_WATER] = 100 * 16 * WATER_FARM;
        // commodityMaxProd[STUFF_FOOD] = 100 *
        //   (ORGANIC_FARM_FOOD_OUTPUT + tech_bonus);
    }

    virtual void initialize() override {
        RegisteredConstruction::initialize();

        this->tech_bonus = (int)((long long int)this->tech
          * ORGANIC_FARM_FOOD_OUTPUT / MAX_TECH_LEVEL);

        commodityMaxProd[STUFF_FOOD] = 100 *
          (ORGANIC_FARM_FOOD_OUTPUT + tech_bonus);
    }

    virtual ~Organic_farm() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;

    int  ugwCount;
    int  max_foodprod;
    int  food_this_month;
    int  food_last_month;
    int  crop_rotation_key;
    int  month_stagger;
    int  tech;
    int  tech_bonus;
};



/** @file lincity/modules/organic_farm.h */
