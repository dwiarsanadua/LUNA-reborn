--[[
  Quest 802: [Party] Terrible Things
  Level Required: 68
  NPC Start: 117, NPC Complete: 117
  Rewards: EXP=994997, Gold=21542
]]

local fsm = require('fsm_engine')

local quest_802 = fsm:new({
    id = 802,
    name = "[Party] Terrible Things",
    level_required = 68,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 802: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 802: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 802: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 89,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 802: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 802: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 802: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 802: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 802: Rewards given")
                self:give_rewards({
                    exp = 994997,
                    gold = 21542,
                })
            end,
            transitions = {},
        },
    },
})

return quest_802