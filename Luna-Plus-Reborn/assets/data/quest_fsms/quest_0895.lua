--[[
  Quest 895: [Party] Heartwarming Theives
  Level Required: 98
  NPC Start: 535, NPC Complete: 535
  Prerequisites: [891]
  Rewards: EXP=8938143, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_895 = fsm:new({
    id = 895,
    name = "[Party] Heartwarming Theives",
    level_required = 98,
    npc_start = 535,
    npc_complete = 535,
    prerequisites = {891},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 895: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 895: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 895: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 291,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 895: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 292,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 895: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 293,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 895: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 895: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 895: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 895: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 895: Rewards given")
                self:give_rewards({
                    exp = 8938143,
                    gold = 77721,
                })
            end,
            transitions = {},
        },
    },
})

return quest_895