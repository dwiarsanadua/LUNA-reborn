--[[
  Quest 719: This is going to be irritating!
  Level Required: 49
  NPC Start: 118, NPC Complete: 118
  Prerequisites: [718]
  Rewards: EXP=204499
]]

local fsm = require('fsm_engine')

local quest_719 = fsm:new({
    id = 719,
    name = "This is going to be irritating!",
    level_required = 49,
    npc_start = 118,
    npc_complete = 118,
    prerequisites = {718},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 719: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 118,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 719: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 719: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 92,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 719: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 87,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 719: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 285,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 719: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 118,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 719: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 719: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 118,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 719: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 719: Rewards given")
                self:give_rewards({
                    exp = 204499,
                })
            end,
            transitions = {},
        },
    },
})

return quest_719