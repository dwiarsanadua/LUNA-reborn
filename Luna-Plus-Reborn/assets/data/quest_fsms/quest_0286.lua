--[[
  Quest 286: [Emblem of Influence] Pure Profit
  Level Required: 70
  NPC Start: 6, NPC Complete: 62
  Prerequisites: [285]
  Rewards: EXP=1050000, Gold=40000
]]

local fsm = require('fsm_engine')

local quest_286 = fsm:new({
    id = 286,
    name = "[Emblem of Influence] Pure Profit",
    level_required = 70,
    npc_start = 6,
    npc_complete = 62,
    prerequisites = {285},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 286: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 286: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 286: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 36,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 286: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 102,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 286: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 286: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 286: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 286: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 286: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 286: Rewards given")
                self:give_rewards({
                    exp = 1050000,
                    gold = 40000,
                    items = {
                        { item_id = 203, count = 25 },
                        { item_id = 30000486, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_286