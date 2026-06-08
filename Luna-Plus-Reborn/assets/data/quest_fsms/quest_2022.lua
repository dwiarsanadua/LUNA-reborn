--[[
  Quest 2022: [Daily] Troll Shaman
  Level Required: 72
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=678002
]]

local fsm = require('fsm_engine')

local quest_2022 = fsm:new({
    id = 2022,
    name = "[Daily] Troll Shaman",
    level_required = 72,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2022: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2022: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2022: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 104,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2022: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2022: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2022: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2022: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2022: Rewards given")
                self:give_rewards({
                    exp = 678002,
                    items = {
                        { item_id = 30000835, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2022