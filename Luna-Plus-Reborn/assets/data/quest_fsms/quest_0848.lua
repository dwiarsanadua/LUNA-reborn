--[[
  Quest 848: [Party] Monsters of the Wreck
  Level Required: 87
  NPC Start: 505, NPC Complete: 505
  Rewards: EXP=3799344
]]

local fsm = require('fsm_engine')

local quest_848 = fsm:new({
    id = 848,
    name = "[Party] Monsters of the Wreck",
    level_required = 87,
    npc_start = 505,
    npc_complete = 505,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 848: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 848: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 848: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 685,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 848: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 693,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 848: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 694,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 848: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 848: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 848: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 848: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 848: Rewards given")
                self:give_rewards({
                    exp = 3799344,
                })
            end,
            transitions = {},
        },
    },
})

return quest_848