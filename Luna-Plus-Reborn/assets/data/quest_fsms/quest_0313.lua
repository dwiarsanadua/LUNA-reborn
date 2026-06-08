--[[
  Quest 313: [C Class] Harpy Elimination
  Level Required: 43
  NPC Start: 80, NPC Complete: 80
  Prerequisites: [311]
  Rewards: EXP=165100, Gold=4500
]]

local fsm = require('fsm_engine')

local quest_313 = fsm:new({
    id = 313,
    name = "[C Class] Harpy Elimination",
    level_required = 43,
    npc_start = 80,
    npc_complete = 80,
    prerequisites = {311},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 313: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 313: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 313: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 30,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 313: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 313: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 313: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 313: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 313: Rewards given")
                self:give_rewards({
                    exp = 165100,
                    gold = 4500,
                    items = {
                        { item_id = 218, count = 50 },
                        { item_id = 30000499, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_313