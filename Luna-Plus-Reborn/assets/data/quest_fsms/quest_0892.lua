--[[
  Quest 892: No Blood of Tears 2
  Level Required: 97
  NPC Start: 536, NPC Complete: 536
  Prerequisites: [891]
  Rewards: EXP=8266754, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_892 = fsm:new({
    id = 892,
    name = "No Blood of Tears 2",
    level_required = 97,
    npc_start = 536,
    npc_complete = 536,
    prerequisites = {891},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 892: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 892: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 892: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 291,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 892: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 292,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 892: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 892: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 892: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 892: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 892: Rewards given")
                self:give_rewards({
                    exp = 8266754,
                    gold = 77721,
                })
            end,
            transitions = {},
        },
    },
})

return quest_892