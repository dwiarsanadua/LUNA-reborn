--[[
  Quest 2037: [Daily]Squishing the Black Widow Clan 2
  Level Required: 87
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=1899672
]]

local fsm = require('fsm_engine')

local quest_2037 = fsm:new({
    id = 2037,
    name = "[Daily]Squishing the Black Widow Clan 2",
    level_required = 87,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2037: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2037: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2037: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 284,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2037: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2037: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2037: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2037: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2037: Rewards given")
                self:give_rewards({
                    exp = 1899672,
                    items = {
                        { item_id = 30000835, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2037