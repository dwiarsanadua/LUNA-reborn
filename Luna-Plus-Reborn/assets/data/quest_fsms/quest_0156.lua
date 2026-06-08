--[[
  Quest 156: The Last Ingredient
  Level Required: 28
  NPC Start: 28, NPC Complete: 28
  Prerequisites: [155]
  Rewards: EXP=29722, Gold=3240
]]

local fsm = require('fsm_engine')

local quest_156 = fsm:new({
    id = 156,
    name = "The Last Ingredient",
    level_required = 28,
    npc_start = 28,
    npc_complete = 28,
    prerequisites = {155},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 156: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 156: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 156: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 50,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 156: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 156: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 156: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 156: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 156: Rewards given")
                self:give_rewards({
                    exp = 29722,
                    gold = 3240,
                    items = {
                        { item_id = 70, count = 1 },
                        { item_id = 21000010, count = 5 },
                        { item_id = 21001505, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_156