--[[
  Quest 959: Bloody Oath
  Level Required: 110
  NPC Start: 128, NPC Complete: 544
  Rewards: EXP=20095498, Gold=106426
]]

local fsm = require('fsm_engine')

local quest_959 = fsm:new({
    id = 959,
    name = "Bloody Oath",
    level_required = 110,
    npc_start = 128,
    npc_complete = 544,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 959: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 959: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 959: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 345,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 959: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 959: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 544,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 959: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 959: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 544,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 959: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 959: Rewards given")
                self:give_rewards({
                    exp = 20095498,
                    gold = 106426,
                })
            end,
            transitions = {},
        },
    },
})

return quest_959