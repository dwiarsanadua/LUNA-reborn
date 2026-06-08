--[[
  Quest 741: [Quest] Decommissioned Butterflies 3
  Level Required: 63
  NPC Start: 117, NPC Complete: 117
  Prerequisites: [740]
  Rewards: EXP=866190
]]

local fsm = require('fsm_engine')

local quest_741 = fsm:new({
    id = 741,
    name = "[Quest] Decommissioned Butterflies 3",
    level_required = 63,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {740},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 741: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 741: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 741: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 99,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 741: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 100,
                    count = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 741: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 101,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 741: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 741: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 741: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 741: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 741: Rewards given")
                self:give_rewards({
                    exp = 866190,
                })
            end,
            transitions = {},
        },
    },
})

return quest_741