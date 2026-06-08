--[[
  Quest 314: [C Class] Harpy Elimination
  Level Required: 43
  NPC Start: 80, NPC Complete: 80
  Prerequisites: [313]
  Rewards: EXP=165100, Gold=4500
]]

local fsm = require('fsm_engine')

local quest_314 = fsm:new({
    id = 314,
    name = "[C Class] Harpy Elimination",
    level_required = 43,
    npc_start = 80,
    npc_complete = 80,
    prerequisites = {313},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 314: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 314: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 314: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 30,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 314: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 314: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 314: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 314: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 314: Rewards given")
                self:give_rewards({
                    exp = 165100,
                    gold = 4500,
                    items = {
                        { item_id = 219, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_314