--[[
  Quest 770: Bear Hunt
  Level Required: 32
  NPC Start: 28, NPC Complete: 28
  Rewards: EXP=39553, Gold=5803
]]

local fsm = require('fsm_engine')

local quest_770 = fsm:new({
    id = 770,
    name = "Bear Hunt",
    level_required = 32,
    npc_start = 28,
    npc_complete = 28,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 770: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 770: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 770: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 50,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 770: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 770: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 770: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 770: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 770: Rewards given")
                self:give_rewards({
                    exp = 39553,
                    gold = 5803,
                })
            end,
            transitions = {},
        },
    },
})

return quest_770