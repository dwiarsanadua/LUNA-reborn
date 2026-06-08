--[[
  Quest 309: [C Class] Orc Warrior Elimination
  Level Required: 41
  NPC Start: 80, NPC Complete: 80
  Prerequisites: [307]
  Rewards: EXP=137745, Gold=4000
]]

local fsm = require('fsm_engine')

local quest_309 = fsm:new({
    id = 309,
    name = "[C Class] Orc Warrior Elimination",
    level_required = 41,
    npc_start = 80,
    npc_complete = 80,
    prerequisites = {307},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 309: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 309: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 309: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 309: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 309: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 309: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 309: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 309: Rewards given")
                self:give_rewards({
                    exp = 137745,
                    gold = 4000,
                    items = {
                        { item_id = 214, count = 1 },
                        { item_id = 214, count = 50 },
                        { item_id = 30000499, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_309