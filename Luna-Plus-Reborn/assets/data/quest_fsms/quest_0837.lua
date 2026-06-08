--[[
  Quest 837: We are all Friends!
  Level Required: 84
  NPC Start: 123, NPC Complete: 129
  Rewards: EXP=1404408
]]

local fsm = require('fsm_engine')

local quest_837 = fsm:new({
    id = 837,
    name = "We are all Friends!",
    level_required = 84,
    npc_start = 123,
    npc_complete = 129,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 837: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 837: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 837: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 837: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 837: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 837: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 837: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 837: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 123,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 837: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 837: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 837: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 837: Rewards given")
                self:give_rewards({
                    exp = 1404408,
                })
            end,
            transitions = {},
        },
    },
})

return quest_837