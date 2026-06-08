--[[
  Quest 561: Quest 561
  Level Required: 1
  NPC Start: 11, NPC Complete: 11
  Prerequisites: [560]
]]

local fsm = require('fsm_engine')

local quest_561 = fsm:new({
    id = 561,
    name = "Quest 561",
    level_required = 1,
    npc_start = 11,
    npc_complete = 11,
    prerequisites = {560},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 561: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 561: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 561: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 392,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 561: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 186,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 561: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 246,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 561: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 247,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 561: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 248,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 561: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 561: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 561: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 561: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 561: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_561