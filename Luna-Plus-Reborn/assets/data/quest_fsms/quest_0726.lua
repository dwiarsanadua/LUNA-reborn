--[[
  Quest 726: Hunters Union Secondary Elimination Order
  Level Required: 77
  NPC Start: 316, NPC Complete: 316
  Rewards: EXP=2252644
]]

local fsm = require('fsm_engine')

local quest_726 = fsm:new({
    id = 726,
    name = "Hunters Union Secondary Elimination Order",
    level_required = 77,
    npc_start = 316,
    npc_complete = 316,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 726: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 316,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 726: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 726: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 250,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 726: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 260,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 726: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 70,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 726: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 316,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 726: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 726: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 316,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 726: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 726: Rewards given")
                self:give_rewards({
                    exp = 2252644,
                })
            end,
            transitions = {},
        },
    },
})

return quest_726